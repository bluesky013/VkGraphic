#pragma once

#include <Log.h>
#include <stdexcept>
#include <vulkan/vulkan.h>


inline void CHECK_RST(const VkResult& rst)
{
    if (rst != VK_SUCCESS) {
        throw std::runtime_error("check result error");
    }
}

#define VKL_CHECK(val, mssage) \
{ \
    if (!val) { \
        throw std::runtime_error(mssage); \
    } \
}