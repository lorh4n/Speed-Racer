#ifndef VMA_WRAPPER_HPP
#define VMA_WRAPPER_HPP

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include <iostream>
#include <stdexcept>

class VmaWrapper {
public: 
   VmaWrapper();
   ~VmaWrapper();

   void initialize(VkDevice device, VkPhysicalDevice physicalDevice, VkInstance instance);

   bool isInitialized() const { return allocator != VK_NULL_HANDLE; }
private:
   VmaAllocator allocator;
   VkDevice device;

   VmaWrapper (const VmaWrapper&) = delete;  // Operador de Deleção C++
   VmaWrapper& operator = (const VmaWrapper&) = delete;
};

#endif