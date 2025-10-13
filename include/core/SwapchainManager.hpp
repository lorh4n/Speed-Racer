#ifndef SWAPCHAIN_MANAGER_HPP
#define SWAPCHAIN_MANAGER_HPP

#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <limits>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include <core/queueManager.hpp>

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

class SwapchainManager {
  public:
	SwapchainManager();
	SwapchainManager(VkDevice &dev, VkPhysicalDevice &physDev, VkSurfaceKHR &surf, GLFWwindow &win, QueueManager &queue) :
	    device(dev), physicalDevice(physDev), surface(surf), window(win), swapchain(VK_NULL_HANDLE), queueManager(queue) {
	}


	bool createSwapchain(uint32_t width, uint32_t height);
	bool createImageViews();
	void cleanup();
	void cleanupSwapchain();
	void recreateSwapchain(uint32_t width, uint32_t height);

	static bool checkDeviceSupportSwapChain(VkPhysicalDevice device);

	// Support details
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR        capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>   presentModes;
	};
	
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, uint32_t width, uint32_t height);
	
	const VkExtent2D &getSwapchainExtent() const { return swapchainExtent; }

	VkFormat getSwapchainImageFormat() const { return swapchainImageFormat; }

	VkSwapchainKHR getSwapchain() const { return swapchain; }

	// FrameBuffers Functions
	bool createFramebuffers(VkRenderPass renderPass);
	const std::vector<VkFramebuffer>& getFramebuffers() const {
		return swapchainFramebuffers;
	}

	~SwapchainManager() { cleanup(); }
  private:
	VkDevice &device;        // Referência, não cópia
	GLFWwindow &window;
	VkSurfaceKHR &surface;
	VkSwapchainKHR swapchain;
	QueueManager &queueManager;
	VkPhysicalDevice &physicalDevice;
	SwapChainSupportDetails querySwapchainSupport();
	
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	
	std::vector<VkFramebuffer> swapchainFramebuffers;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
};

#endif