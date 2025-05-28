#ifndef QUEUE_MANAGER_HPP
#define QUEUE_MANAGER_HPP

#include <optional>
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>
#include <stdexcept>

enum class QueueType {
    GRAPHICS,
    PRESENT,
    COMPUTE, // Add more types as needed
    TRANSFER
};

struct QueueFamilyInfo {
   uint32_t index;
   uint32_t queueCount; 
   VkQueueFlags flags;   // Queue capabilities (e.g., VK_QUEUE_GRAPHICS_BIT)
   bool supportsPresent;
};

struct QueueFamilyIndices {
   std::optional<uint32_t> graphicsFamily;
   std::optional<uint32_t> presentFamily;
   bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value();}
};

class QueueManager {
public:
   // Store queue family info for reuse
   void initialize(VkPhysicalDevice device, VkSurfaceKHR surface);

   // Retrieve queue handle for a specific type and index
   VkQueue getQueue(VkDevice device, QueueType type, uint32_t queueIndex = 0);
   // Get queue family info for logical device creation
   const std::unordered_map<QueueType, QueueFamilyInfo>& getQueueFamilies() const;

   struct QueueRequirements {
      QueueType type;
      uint32_t minQueueCount = 1;
      VkQueueFlags requiredFlags = 0;
      bool requiresPresent = false;
   };
   // Validate if queue families meet requirements
   static bool areQueueFamiliesSufficient(
      const std::unordered_map<QueueType, QueueFamilyInfo>& families,
      const std::vector<QueueRequirements>& requirements
   );
private: 
   std::unordered_map<QueueType, QueueFamilyInfo> queueFamilies;
       // Find queue families for a physical device
   static std::unordered_map<QueueType, QueueFamilyInfo> findQueueFamilies(
        VkPhysicalDevice device, VkSurfaceKHR surface);
};

#endif