cmake_minimum_required(VERSION 3.22.1)

project(Speed_Racer VERSION 0.0.1)

# Flags de debug
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_BUILD_TYPE Debug)

find_package(Vulkan REQUIRED)

add_executable(Speed_Racer 
   src/app/main.cpp
   src/core/VulkanManager.cpp
   src/core/WindowManager.cpp
   src/core/VulkanTools/VulkanTools.cpp
   src/core/physicalDevice.cpp
   src/core/logicalDevice.cpp
   src/core/queueManager.cpp
   src/core/SwapchainManager.cpp
   src/core/ShaderManager.cpp
)

target_include_directories(Speed_Racer PRIVATE 
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/include/VulkanUtils
   ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/include
   ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
)

target_link_libraries(Speed_Racer 
    Vulkan::Vulkan
    glfw
    GL
    X11
    pthread
    dl
    assimp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()