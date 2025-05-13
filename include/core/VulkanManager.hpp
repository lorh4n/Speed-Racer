#ifndef VULKAN_MANAGER_HPP
#define VULKAN_MANAGER_HPP

#include <iostream>
#include <stdexcept>

#include "WindowManager.hpp"

class VulkanManager {
public:
    VulkanManager(int width, int height, const char* title);
    ~VulkanManager();
    void run();

private:
    WindowManager window;
    VkInstance instance;
    VkSurfaceKHR surface;

    void initVulkan();
    void mainLoop();
    void cleanup();

    void createInstance();
};

#endif