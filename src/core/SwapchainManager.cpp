#include <core/SwapchainManager.hpp>


bool checkDeviceSupportSwapChain(VkPhysicalDevice device) {
    return true;
}

SwapchainManager::SwapChainSupportDetails SwapchainManager::querySwapchainSupport() {
    SwapchainManager::SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool SwapchainManager::checkDeviceSupportSwapChain(VkPhysicalDevice device) {
    std::cout << "- Verifing if Device Support Swap Chain";
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    if (requiredExtensions.empty()){
        std::cout << ": OK" << std::endl;
    } else {
        std::cout << ": ERROR" << std::endl;
    }

    

    return requiredExtensions.empty();
}

//  bool SwapchainManager::createSwapchain(uint32_t width, uint32_t height) {
//     SwapChainSupportDetails swapChainSupport = SwapchainManager::querySwapchainSupport();

//     VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
//     VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
//     VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

//  }