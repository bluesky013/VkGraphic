#pragma once
#include <vulkan/vulkan.h>

namespace vkl {
class Device;

class Swapchain {
public:
    Swapchain(Device& dev)
        : device(dev),
          surface(VK_NULL_HANDLE),
          swapchain(VK_NULL_HANDLE)
    {}
    ~Swapchain() {}

    void init(void* handle);

private:
    Device& device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
};

}