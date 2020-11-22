
project(Test)

file(GLOB TEST_SRC *)

include_directories(${PROJROOT}/Lib/include)
include_directories(${PROJROOT}/VkCore/include)

add_executable(Test
    ${TEST_SRC}
)

# set(GLFW_LIB "G:/3rd/glfw-3.3.2/glfw-3.3.2/build/lib/glfw3.lib")
# include_directories("G:/3rd/glfw-3.3.2/glfw-3.3.2/build/include")

target_link_libraries(Test
    VkCore
)