cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(VkGraphic)

set(PROJROOT ${CMAKE_SOURCE_DIR})

include_directories(${Vulkan_INCLUDE_DIR})

add_subdirectory(VkCore)
add_subdirectory(Test)