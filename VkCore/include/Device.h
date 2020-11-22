#pragma once

#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

#define DEBUG_LAYER

namespace vkl {

class Device {
public:
    Device() : alloc(nullptr) {}
    ~Device() {}

    void init();

    struct QueueFamily {
        std::optional<uint32_t> graphics;
        std::optional<uint32_t> present;
        std::optional<uint32_t> compute;
    };

    struct PhysicalDevice {
        VkPhysicalDevice devInfo;
        std::vector<VkQueueFamilyProperties> props;
    };

private:
    void CreateInstance();
    void InitPhyDevices();
    void CreateDevice();

    VkInstance instance;
    VkPhysicalDevice phyDev;
    VkDevice dev;
    std::vector<VkLayerProperties> layers;
    std::vector<PhysicalDevice> phyDevs;
#ifdef DEBUG_LAYER
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
    VkAllocationCallbacks *alloc;
    QueueFamily queueFamily;
};

}