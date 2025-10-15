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
#include <core/VmaWrapper.hpp>


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

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    const int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    VmaWrapper vmaWrapper;


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
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    void createSyncObjects();
    void setupVmaWrapper();

    void recreateSwapChain();
    void cleanup();


    static void framebufferResizeCallback(GLFWwindow* window, int width, int height); // callback estático para GLFW
};

#endif
