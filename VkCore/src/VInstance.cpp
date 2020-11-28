#include <VInstance.h>
#include <set>
#include <mutex>
#include <Device.h>
#include "Wrap.h"
#include "Result.h"
#ifdef _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace vkl {
VkAllocationCallbacks* VInstance::alloc = nullptr;

const std::vector<const char*> g_extensions = {
    "VK_KHR_surface",
#ifdef _WIN32
    "VK_KHR_win32_surface",
#endif
#ifdef VKL_DEBUG_LAYER
    "VK_EXT_debug_utils"
#endif
};

const std::vector<const char*> g_deviceExt = {
    "VK_KHR_swapchain"
};

static void CheckExtension()
{
    uint32_t extCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> exts(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, exts.data());
}

static void CheckLayers(std::vector<VkLayerProperties>& layers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    layers.resize(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
}


const std::vector<const char*> g_layers = {
#ifdef VKL_DEBUG_LAYER
    "VK_LAYER_KHRONOS_validation"
#endif
};

#ifdef VKL_DEBUG_LAYER
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    return VK_FALSE;
}
#endif


void VInstance::createInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Graphic";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Worm Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &appInfo,
        static_cast<uint32_t>(g_layers.size()),
        g_layers.data(),
        static_cast<uint32_t>(g_extensions.size()),
        g_extensions.data(),
    };
    CheckExtension();
    CheckLayers(layers);
#ifdef VKL_DEBUG_LAYER
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif
    CHECK_RST(vkCreateInstance(&createInfo, alloc, &instance));

#ifdef VKL_DEBUG_LAYER
    CHECK_RST(Wrap::vkCreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, alloc, &debugMessenger));
#endif
}

void VInstance::initPhyDevices()
{
    // choose physical device
    uint32_t count;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    phyDevs.resize(count);
    std::vector<VkPhysicalDevice> pDevs(count);
    vkEnumeratePhysicalDevices(instance, &count, pDevs.data());
    VKL_CHECK(!pDevs.empty(), "physical device empty");
    for (uint32_t i = 0; i < count; ++i) {
        phyDevs[i].dev = pDevs[i];

        // queue family index
        uint32_t queueCount;
        vkGetPhysicalDeviceQueueFamilyProperties(pDevs[i], &queueCount, nullptr);
        phyDevs[i].props.resize(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(pDevs[i], &queueCount, phyDevs[i].props.data());
        vkGetPhysicalDeviceFeatures(pDevs[i], &phyDevs[i].feature);
    }
}

Device* VInstance::createDevice(uint32_t index)
{
    Device* dev = new Device();
    Device::Context& context = dev->context;
    // choose device;
    auto& phyInfo = phyDevs[index];
    context.phyDev = phyInfo.dev;
    int i = 0;
    for (auto& p : phyInfo.props) {
        if (p.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            context.queueFamily.graphics = i;
        }
        if (p.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            context.queueFamily.transfer = i;
        }
        if (p.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            context.queueFamily.compute = i;
        }
    }
    VKL_CHECK(context.queueFamily.graphics.has_value() && context.queueFamily.transfer.has_value() &&
        context.queueFamily.compute.has_value(), "queue family index check failed.");

    // device queue.
    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    std::set<uint32_t> queueFamilies = {
        context.queueFamily.graphics.value(),
        context.queueFamily.transfer.value(),
        context.queueFamily.compute.value()
    };
    std::vector<float> priority = {
        1.0f
    };
    for (auto qf : queueFamilies) {
        VkDeviceQueueCreateInfo queueInfo = {};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = qf;
        queueInfo.queueCount = static_cast<uint32_t>(priority.size());
        queueInfo.pQueuePriorities = priority.data();
        queueInfos.emplace_back(queueInfo);
    }

    // device feature.
    VkPhysicalDeviceFeatures feature = {};
    feature.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo devInfo = {};
    devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    devInfo.pQueueCreateInfos = queueInfos.data();
    devInfo.pEnabledFeatures = &feature;
    devInfo.enabledExtensionCount = static_cast<uint32_t>(g_deviceExt.size());
    devInfo.ppEnabledExtensionNames = g_deviceExt.data();
#ifdef VKL_DEBUG_LAYER
    devInfo.enabledLayerCount = static_cast<uint32_t>(g_layers.size());
    devInfo.ppEnabledLayerNames = g_layers.data();
#endif
    CHECK_RST(vkCreateDevice(context.phyDev, &devInfo, alloc, &context.device));

    // queue
    vkGetDeviceQueue(context.device, context.queueFamily.graphics.value(), 0, &context.queues.graphic);
    vkGetDeviceQueue(context.device, context.queueFamily.transfer.value(), 0, &context.queues.transfer);
    vkGetDeviceQueue(context.device, context.queueFamily.compute.value(), 0, &context.queues.compute);
    return dev;
}

void VInstance::init()
{
    createInstance();
    initPhyDevices();
}

VInstance* g_instance = nullptr;
std::mutex g_mutex;
VInstance* VInstance::getInstance()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_instance == nullptr) {
        g_instance = new VInstance();
        g_instance->init();
    }
    return g_instance;
}

}