#include <VInstance.h>
#include <Device.h>

using namespace vkl;

VInstance instance;

int main() {
    instance.init();

    Device* dev = instance.createDevice(0);

    return 0;
}