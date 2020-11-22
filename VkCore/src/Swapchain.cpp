#include <Swapchain.h>
#ifdef _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace vkl {

void Swapchain::init(void* handle)
{
    // init surface
#ifdef _WIN32
    //vkCreateWin32SurfaceKHR()
#endif
}

}