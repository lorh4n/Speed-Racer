#ifndef SWAPCHAIN_MANAGER_HPP
#define SWAPCHAIN_MANAGER_HPP

#include <vector>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <set>
#include <string>
#include <iostream>

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


class SwapchainManager {
public:
   SwapchainManager(VkDevice& dev, VkPhysicalDevice& physDev, VkSurfaceKHR& surf)
   : device(dev), physicalDevice(physDev), surface(surf), swapchain(VK_NULL_HANDLE) {}

   bool createSwapchain(uint32_t width, uint32_t height);
   bool createImageViews();
   void cleanup();
   // void recreateSwapchain(uint32_t width, uint32_t height);

   static bool checkDeviceSupportSwapChain(VkPhysicalDevice device);
   
   // Support details
   struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
   };      
   private:
   VkDevice& device;              // Referência, não cópia
   VkPhysicalDevice& physicalDevice;
   VkSurfaceKHR& surface;
   VkSwapchainKHR swapchain;
   
   SwapChainSupportDetails querySwapchainSupport();
   // std::vector<VkImage> swapchainImages;
   // std::vector<VkImageView> swapchainImageViews;
   // VkFormat swapchainImageFormat;
   // VkExtent2D swapchainExtent;

   // VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
   // VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
   // VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
};

#endif