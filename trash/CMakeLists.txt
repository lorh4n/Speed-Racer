cmake_minimum_required(VERSION 3.22.1)

project(codebase VERSION 0.0.1)

find_package(Vulkan REQUIRED)

add_executable(codebase 
   code_base.cpp
)


target_include_directories(codebase PRIVATE 
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/include/VulkanUtils
   ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include
   ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
   # /usr/include/assimp
)

target_link_libraries(codebase 
    Vulkan::Vulkan
    glfw
    GL
    X11
    pthread
    dl
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()