#include <Device.h>
#include <Swapchain.h>

namespace vkl {

Swapchain* Device::createSwapchain(void* handle)
{
	Swapchain* swc = new Swapchain(*this);
	swc->init(handle);
	return swc;
}

}