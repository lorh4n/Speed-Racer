#ifndef LOGICAL_DEVICE_HPP
#define LOGICAL_DEVICE_HPP

#include <vulkan/vulkan.h>
#include <set>
#include "physicalDevice.hpp"
#include <core/queueManager.hpp>

class LogicalDeviceCreator {
   public:
      struct DeviceQueue {
         VkQueue graphicsQueue;
         VkQueue presentQueue;
      };
      static std::pair<VkDevice, DeviceQueue> create(
        VkPhysicalDevice physicalDevice,
        QueueManager& queueManager,
        bool enableValidationLayers,
        const std::vector<const char*>& validationLayers,
        const std::vector<const char*>& deviceExtensions
    );
};

#endif