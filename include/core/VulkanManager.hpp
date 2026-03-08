#ifndef VULKAN_MANAGER_HPP
#define VULKAN_MANAGER_HPP

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "VulkanUtils/VulkanTools.hpp"
#include <core/ResourceTypes.hpp>

#include <core/BufferManager.hpp>
#include <core/CommandManager.hpp>
#include <core/PipelineManager.hpp>
#include <core/ResourceManager.hpp>
#include <core/ShaderManager.hpp>
#include <core/SwapchainManager.hpp>
#include <core/VmaWrapper.hpp>
#include <core/WindowManager.hpp>
#include <core/logicalDevice.hpp>
#include <core/physicalDevice.hpp>
#include <core/queueManager.hpp>
#include <core/Mesh.hpp>

// Coordena a criação da instância Vulkan, ciclo da janela e liberação dos recursos.
class VulkanManager {
  public:
	VulkanManager(int width, int height, const char *title);
	~VulkanManager();
	void run();

  private:
	WindowManager                     window;
	VkInstance                        instance;
	VkSurfaceKHR                      surface;
	VkDebugUtilsMessengerEXT          debugMessenger;
	VkPhysicalDevice                  physicalDevice;
	VkDevice                          device;
	QueueManager                      queueManager;
	LogicalDeviceCreator::DeviceQueue queues;
	std::unique_ptr<SwapchainManager> swapchainManager;        // Mantém a posse exclusiva do swapchain, garantindo liberação automática na destruição.
	VkRenderPass                      renderPass;
	VkPipelineLayout                  graphicsPipelineLayout;
	VkPipeline                        graphicsPipeline;
	std::unique_ptr<CommandManager>   commandManager;
	std::vector<VkCommandBuffer>      commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence>     inFlightFences;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t  currentFrame         = 0;
	bool      framebufferResized   = false;

	VmaWrapper vmaWrapper;

	std::unique_ptr<ResourceManager> resourceManager;
	std::unique_ptr<BufferManager>   bufferManager;

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
	void createResourceManager();
	void createBufferManager();

	// TESTES DE MESH E RENDERING
	std::unique_ptr<Mesh> cubeMesh;
	std::unique_ptr<Mesh> triangleMesh;
	void createCube();
	void createTriangle();

	void recreateSwapChain();
	void cleanup();

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height);        // callback estático para GLFW
};

#endif
