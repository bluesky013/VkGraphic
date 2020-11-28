#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace vkl {
class Device;

struct SwapChainInfo {
    uint32_t width;
    uint32_t height;
    VkFormat format;
    VkColorSpaceKHR colorSpace;
};

class Swapchain {
public:
    Swapchain(Device& dev)
        : device(dev),
          surface(VK_NULL_HANDLE),
          swapchain(VK_NULL_HANDLE)
    {}
    ~Swapchain();

    bool init(void* handle, const SwapChainInfo& swc);

private:
    Device& device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkSurfaceCapabilitiesKHR cap;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presents;
    SwapChainInfo info;
    std::vector<VkImage> images;
    std::vector<VkImageView> views;
};

}