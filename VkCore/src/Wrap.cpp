#include "Wrap.h"

PFN_vkCreateDebugUtilsMessengerEXT pCreateDebugMess = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT pDestroyDebugMess = nullptr;

namespace vkl {

VkResult Wrap::vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    if (pCreateDebugMess == nullptr) {
        pCreateDebugMess = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    }
    if (pCreateDebugMess != nullptr) {
        return pCreateDebugMess(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Wrap::vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    if (pDestroyDebugMess == nullptr) {
        pDestroyDebugMess = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    }
    if (pDestroyDebugMess != nullptr) {
        pDestroyDebugMess(instance, debugMessenger, pAllocator);
    }
}

}