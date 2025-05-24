#ifndef PHYSICAL_DEVICE_HPP
#define PHYSICAL_DEVICE_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <iostream>


class PhysicalDeviceSelector {
   public:
      static VkPhysicalDevice select(VkInstance instance);
      static bool isDeviceSuitable(VkPhysicalDevice device);
};

#endif
