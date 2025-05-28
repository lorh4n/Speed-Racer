#include <core/physicalDevice.hpp>


VkPhysicalDevice PhysicalDeviceSelector::select(VkInstance instance, VkSurfaceKHR surface, QueueManager& queueManager) {
   uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
   if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
   std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
   for (const auto& device : devices) {
      if (isDeviceSuitable(device, surface, queueManager)) {
			return device;
		}
	}

	throw std::runtime_error("failed to find a suitable GPU!");
}



bool PhysicalDeviceSelector::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, QueueManager& queueManager) {
   // Initialize QueueManager for this device
    queueManager.initialize(device, surface);
    
    // Define requirements (same as in QueueManager::initialize for consistency)
    std::vector<QueueManager::QueueRequirements> requirements = {
        {QueueType::GRAPHICS, 1, VK_QUEUE_GRAPHICS_BIT, false},
        {QueueType::PRESENT, 1, 0, true}
    };

    // Check queue family support
    return QueueManager::areQueueFamiliesSufficient(queueManager.getQueueFamilies(), requirements);
    // Add more checks (e.g., extensions, features) as needed
}