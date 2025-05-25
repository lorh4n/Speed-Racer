#include <core/queueManager.hpp>

void QueueManager::initialize(VkPhysicalDevice device, VkSurfaceKHR surface) {
   queueFamilies = findQueueFamilies(device, surface);

   std::vector<QueueRequirements> requirements = {
      {QueueType::GRAPHICS, 1, VK_QUEUE_GRAPHICS_BIT, false},
      {QueueType::PRESENT, 1, 0, true}
   };

   if (!areQueueFamiliesSufficient(queueFamilies, requirements)) {
      throw std::runtime_error("Physical device does not support required queue families");
   }
}

std::unordered_map<QueueType, QueueFamilyInfo> QueueManager::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {

   std::unordered_map<QueueType, QueueFamilyInfo> families;

   uint32_t queueFamilyCount = 0;
   vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
   std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
   vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

   for (uint32_t i = 0; i < queueFamilyCount; ++i) {
      const auto& props = queueFamilyProperties[i];
      
      // Check for graphics queue
      if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
         families[QueueType::GRAPHICS] = {i, props.queueCount, props.queueFlags, false};
      }
      
      // Check for compute queue
      if (props.queueFlags & VK_QUEUE_COMPUTE_BIT) {
         families[QueueType::COMPUTE] = {i, props.queueCount, props.queueFlags, false};
      }
      
      // Check for transfer queue
      if (props.queueFlags & VK_QUEUE_TRANSFER_BIT) {
         families[QueueType::TRANSFER] = {i, props.queueCount, props.queueFlags, false};
      }
      
      // Check for present support
      VkBool32 presentSupport = VK_FALSE;
      if (surface != VK_NULL_HANDLE) {
         vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
         if (presentSupport) {
               families[QueueType::PRESENT] = {i, props.queueCount, props.queueFlags, true};
         }
      }
   }
   
   return families;
}



bool QueueManager::areQueueFamiliesSufficient(
    const std::unordered_map<QueueType, QueueFamilyInfo>& families,
    const std::vector<QueueRequirements>& requirements) {
    for (const auto& req : requirements) {
        auto it = families.find(req.type);
        if (it == families.end()) {
            return false; // Missing required queue type
        }
        const auto& info = it->second;
        if (info.queueCount < req.minQueueCount ||
            (req.requiredFlags & info.flags) != req.requiredFlags ||
            (req.requiresPresent && !info.supportsPresent)) {
            return false; // Queue family doesn't meet requirements
        }
    }
    return true;
}


VkQueue QueueManager::getQueue(VkDevice device, QueueType type, uint32_t queueIndex) {
    auto it = queueFamilies.find(type);
    if (it == queueFamilies.end() || queueIndex >= it->second.queueCount) {
        throw std::runtime_error("Invalid queue type or index");
    }
    VkQueue queue;
    vkGetDeviceQueue(device, it->second.index, queueIndex, &queue);
    if (queue == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to retrieve queue handle");
    }
    return queue;
}

const std::unordered_map<QueueType, QueueFamilyInfo>& QueueManager::getQueueFamilies() const {
    return queueFamilies;
}