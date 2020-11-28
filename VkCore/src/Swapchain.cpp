#include <Swapchain.h>
#include <Device.h>
#include <VInstance.h>
#include "Result.h"
#ifdef _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace vkl {

Swapchain::~Swapchain()
{
    vkDeviceWaitIdle(device.context.device);
    if (swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device.context.device, swapchain, VInstance::alloc);
    }

    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(VInstance::getInstance()->instance, surface, VInstance::alloc);
    }
}

bool Swapchain::init(void* handle, const SwapChainInfo& swc)
{
    info = swc;
    VInstance* inst = VInstance::getInstance();
    // init surface
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    info.hinstance = GetModuleHandle(nullptr);
    info.hwnd = (HWND)(handle);
    CHECK_RST(vkCreateWin32SurfaceKHR(inst->instance, &info, VInstance::alloc, &surface));
#endif

    // surface capabitity
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.context.phyDev, surface, &cap);
    uint32_t count;

    // surface format
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.context.phyDev, surface, &count, nullptr);
    formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device.context.phyDev, surface, &count, formats.data());

    // present mode
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.context.phyDev, surface, &count, nullptr);
    presents.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device.context.phyDev, surface, &count, presents.data());

    auto fIter = std::find_if(formats.begin(), formats.end(), [&swc](VkSurfaceFormatKHR& fmt) {
        return (fmt.format == swc.format && fmt.colorSpace == swc.colorSpace);
    });
    if (fIter == formats.end()) {
        return false;
    }

    VkBool32 support = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device.context.phyDev, device.context.queueFamily.transfer.value(), surface, &support);
    if (!support) {
        return false;
    }

    uint32_t imageCount = std::min(cap.minImageCount + 1, cap.maxImageCount);
    VkExtent2D ext = {
        swc.width,
        swc.height
    };
    ext.width = std::max(cap.minImageExtent.width, std::min(cap.maxImageExtent.width, ext.width));
    ext.height = std::max(cap.minImageExtent.height, std::min(cap.maxImageExtent.height, ext.height));
    
    VkSwapchainCreateInfoKHR swcInfo = {};
    swcInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swcInfo.surface = surface;
    swcInfo.minImageCount = imageCount;
    swcInfo.imageFormat = swc.format;
    swcInfo.imageColorSpace = swc.colorSpace;
    swcInfo.imageExtent = ext;
    swcInfo.imageArrayLayers = 1;
    swcInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swcInfo.preTransform = cap.currentTransform;
    swcInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swcInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    swcInfo.clipped = VK_TRUE;

    swcInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (device.context.queueFamily.graphics != device.context.queueFamily.transfer) {
        uint32_t indices[] = { device.context.queueFamily.graphics.value(), device.context.queueFamily.transfer.value() };
        swcInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swcInfo.queueFamilyIndexCount = sizeof(indices) / sizeof(uint32_t);
        swcInfo.pQueueFamilyIndices = indices;
    }

    CHECK_RST(vkCreateSwapchainKHR(device.context.device, &swcInfo, VInstance::alloc, &swapchain));

    vkGetSwapchainImagesKHR(device.context.device, swapchain, &count, nullptr);
    images.resize(count);
    vkGetSwapchainImagesKHR(device.context.device, swapchain, &count, images.data());

    views.resize(count);
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swc.format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    for (uint32_t i = 0; i < count; i++) {
        viewInfo.image = images[i];
        CHECK_RST(vkCreateImageView(device.context.device, &viewInfo, VInstance::alloc, &views[i]));
    }

    return true;
}

}