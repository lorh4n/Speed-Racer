cmake_minimum_required(VERSION 3.22.1)

project(Speed_Racer VERSION 0.0.1)

find_package(Vulkan REQUIRED)

add_executable(Speed_Racer 
   src/app/main.cpp
   src/core/VulkanManager.cpp
)

target_link_libraries(Speed_Racer Vulkan::Vulkan)

target_include_directories(Speed_Racer PRIVATE include)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()