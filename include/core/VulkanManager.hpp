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
#include <core/ShaderManager.hpp>
#include <core/PipelineManager.hpp>
#include <core/CommandManager.hpp>


// Coordena a criação da instância Vulkan, ciclo da janela e liberação dos recursos.
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
    std::unique_ptr<SwapchainManager> swapchainManager;     // Mantém a posse exclusiva do swapchain, garantindo liberação automática na destruição.
    VkRenderPass renderPass;
    VkPipelineLayout graphicsPipelineLayout;
    VkPipeline graphicsPipeline;
    std::unique_ptr<CommandManager> commandManager;
    std::vector<VkCommandBuffer> commandBuffers;    


    void initVulkan();
    void mainLoop();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createInstance();
    void createSurface();
    void setupSwapChain();
    void createLogicalDevice();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();    

    void cleanup();
};

#endif
