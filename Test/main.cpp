#include <VInstance.h>
#include <Device.h>
#include <Swapchain.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

using namespace vkl;

VInstance* instance = nullptr;

void resize(GLFWwindow*, int, int)
{

}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, resize);

    HWND hwnd = glfwGetWin32Window(window);

    instance = VInstance::getInstance();
    Device* dev = instance->createDevice(0);

    SwapChainInfo swcInfo = {};
    glfwGetFramebufferSize(window, (int*)&swcInfo.width, (int*)&swcInfo.height);
    swcInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
    swcInfo.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    Swapchain* swc = dev->createSwapchain(hwnd, swcInfo);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}