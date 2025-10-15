#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h" 


struct BufferCreateInfo {
   VkDeviceSize size;
   VkBufferUsageFlags usage;
   VmaMemoryUsage memoryUsage;
};

// struct ImageCreateInfo { ... };