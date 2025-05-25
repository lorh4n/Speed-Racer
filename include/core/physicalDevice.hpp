#ifndef PHYSICAL_DEVICE_HPP
#define PHYSICAL_DEVICE_HPP

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include <stdexcept>
#include <iostream>

#include <core/queueManager.hpp>

class PhysicalDeviceSelector {
public:
    static VkPhysicalDevice select(VkInstance instance, VkSurfaceKHR surface, QueueManager& queueManager);
    static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, QueueManager& queueManager);
};

#endif
