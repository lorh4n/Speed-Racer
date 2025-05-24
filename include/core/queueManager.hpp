#ifndef QUEUE_MANAGER_HPP
#define QUEUE_MANAGER_HPP

#include <optional>
#include <vulkan/vulkan.h>

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
   bool supportPresent;
};

struct QueueFamilyIndices {
   std::optional<uint32_t> graphicsFamily;
   std::optional<uint32_t> presentFamily;
   bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value();}
};

class QueueManager {
   public:
   static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};

#endif