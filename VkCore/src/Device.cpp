#include <Device.h>
#include "Wrap.h"
#include "Result.h"
#ifdef _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace vkl {
const std::vector<const char*> g_extensions = {
    "VK_KHR_surface",
#ifdef _WIN32
    "VK_KHR_win32_surface",
#endif
#ifdef DEBUG_LAYER
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
#ifdef DEBUG_LAYER
    "VK_LAYER_KHRONOS_validation"
#endif
};

#ifdef DEBUG_LAYER
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    return VK_FALSE;
}
#endif


void Device::CreateInstance()
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
#ifdef DEBUG_LAYER
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = debugCallback;
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#endif
    CheckRst(vkCreateInstance(&createInfo, alloc, &instance));

#ifdef DEBUG_LAYER
    CheckRst(Wrap::vkCreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, alloc, &debugMessenger));
#endif
}

void Device::InitPhyDevices()
{
    // choose physical device
    uint32_t count;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    phyDevs.resize(count);
    std::vector<VkPhysicalDevice> pDevs(count);
    vkEnumeratePhysicalDevices(instance, &count, pDevs.data());
    VKL_CHECK(!pDevs.empty(), "physical device empty");
    for (uint32_t i = 0; i < count; ++i) {
        phyDevs[i].devInfo = pDevs[i];

        // queue family index
        vkGetPhysicalDeviceQueueFamilyProperties(pDevs[i], &count, nullptr);
        phyDevs[i].props.resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(pDevs[i], &count, phyDevs[i].props.data());
    }
}

void Device::init()
{
    CreateInstance();
    InitPhyDevices();
}

}