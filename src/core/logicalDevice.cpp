#include <core/logicalDevice.hpp>

std::pair<VkDevice, LogicalDeviceCreator::DeviceQueue> LogicalDeviceCreator::create( VkPhysicalDevice physicalDevice, 
    const QueueFamilyIndices& indices,
    bool enableValidationLayers,
    const std::vector<const char*>& validationLayers) {

      std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
      std::set<uint32_t> uniqueQueueFamilies = {
         indices.graphicsFamily.value(),
         indices.presentFamily.value()
      };

      float queuePriority = 1.0f;
      for (uint32_t queueFamily : uniqueQueueFamilies) {
         VkDeviceQueueCreateInfo queueCreateInfo{};
         queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
         queueCreateInfo.queueFamilyIndex = queueFamily;
         queueCreateInfo.queueCount = 1;
         queueCreateInfo.pQueuePriorities = &queuePriority;
         queueCreateInfos.push_back(queueCreateInfo);
      }

      VkPhysicalDeviceFeatures deviceFeatures{};

      VkDeviceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
      createInfo.pQueueCreateInfos = queueCreateInfos.data();
      createInfo.pEnabledFeatures = &deviceFeatures;
      createInfo.enabledExtensionCount = 0;

      if (enableValidationLayers) {
         createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
         createInfo.ppEnabledLayerNames = validationLayers.data();
      } else {
        createInfo.enabledLayerCount = 0;
      }
      VkDevice device;
      
      if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
         throw std::runtime_error("failed to create Logical Device");
      } 

      DeviceQueue queues;
      vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &queues.graphicsQueue);
      vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &queues.presentQueue);

      return {device, queues};
}