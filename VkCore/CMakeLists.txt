project(VkCore)

find_package(VULKAN)
option(VKL_DEBUG_LAYER "option for Vulkan DebugLayer" ON)

file(GLOB VK_CORE_SRC
    include/*
    src/*
)

message("include library: " ${Vulkan_INCLUDE_DIR})
message("link library: " ${Vulkan_LIBRARY})

include_directories(${PROJROOT}/Lib/include)
include_directories(include)

add_library(VkCore STATIC
    ${VK_CORE_SRC}
)

target_link_libraries(VkCore
    ${Vulkan_LIBRARY}
)