#ifndef VULKAN_MANAGER_HPP
#define VULKAN_MANAGER_HPP

#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>

#include "VulkanUtils/VulkanTools.hpp"

#include <core/WindowManager.hpp>
#include <core/logicalDevice.hpp>
#include <core/queueManager.hpp>
#include <core/physicalDevice.hpp>
#include <core/SwapchainManager.hpp>


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
    std::unique_ptr<SwapchainManager> swapchainManager;  // Use pointer instead // Tenho que estudar depois doq ue esse unique_ptr

    void initVulkan();
    void mainLoop();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createInstance();
    void createSurface();
    void setupSwapChain();
    void createLogicalDevice();
    void createGraphicsPipeline();

    void cleanup();
};

#endif