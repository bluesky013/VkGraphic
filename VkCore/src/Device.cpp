#include <Device.h>
#include <Swapchain.h>

namespace vkl {

Swapchain* Device::createSwapchain(void* handle, const SwapChainInfo& info)
{
	Swapchain* swc = new Swapchain(*this);
	if (!swc->init(handle, info)) {
		delete swc;
		return nullptr;
	}
	return swc;
}

}