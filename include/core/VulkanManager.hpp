#ifndef VULKAN_MANAGER_HPP
#define VULKAN_MANAGER_HPP

#include <iostream>
#include <stdexcept>
#include <vector>

#include "VulkanUtils/VulkanTools.hpp"

#include <core/WindowManager.hpp>
#include <core/logicalDevice.hpp>
#include <core/queueManager.hpp>
#include <core/physicalDevice.hpp>


class VulkanManager {
public:
    VulkanManager(int width, int height, const char* title);
    ~VulkanManager();
    void run();

private:
    WindowManager window;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    QueueManager queueManager;
    LogicalDeviceCreator::DeviceQueue queues;

    void initVulkan();
    void mainLoop();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createInstance();
    void cleanup();
};

#endif