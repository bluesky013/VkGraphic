#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#define VKL_DEBUG_LAYER

namespace vkl {
class Device;

class VInstance {
public:
    static VInstance* getInstance();

    struct PhysicalDevice {
        VkPhysicalDevice dev;
        std::vector<VkQueueFamilyProperties> props;
        VkPhysicalDeviceFeatures feature;
    };

    Device* createDevice(uint32_t index);

    static VkAllocationCallbacks* alloc;
private:
    VInstance()
        : instance(VK_NULL_HANDLE)
#ifdef VKL_DEBUG_LAYER
         ,debugMessenger(VK_NULL_HANDLE)
#endif
    {}
    ~VInstance() {}

    void init();

    void createInstance();
    void initPhyDevices();

    VkInstance instance;
#ifdef VKL_DEBUG_LAYER
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
    std::vector<VkLayerProperties> layers;
    std::vector<PhysicalDevice> phyDevs;
};

}