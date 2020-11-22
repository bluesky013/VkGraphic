#pragma once

#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

namespace vkl {
class Swapchain;

class Device {
public:
    Device() {}
    ~Device() {}

    Swapchain* createSwapchain(void* handle);

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
private:
    friend class VInstance;
    Context context;
};

}