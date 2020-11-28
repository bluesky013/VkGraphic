#pragma once

#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

namespace vkl {
class Swapchain;
struct SwapChainInfo;

class Device {
public:
    Device() {}
    ~Device() {}

    Swapchain* createSwapchain(void* handle, const SwapChainInfo& info);

    struct QueueFamily {
        std::optional<uint32_t> graphics;
        std::optional<uint32_t> transfer;
        std::optional<uint32_t> compute;
    };

    struct Queue {
        VkQueue graphic;
        VkQueue transfer;
        VkQueue compute;
    };

    struct Context {
        VkPhysicalDevice phyDev;
        QueueFamily queueFamily;
        VkDevice device;
        Queue queues;
    };
    Context context;
};

}