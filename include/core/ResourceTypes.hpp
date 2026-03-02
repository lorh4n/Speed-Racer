#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h" 


struct BufferCreateInfo {
   VkDeviceSize size;
   VkBufferUsageFlags usage;
   VmaMemoryUsage memoryUsage;
};

struct Vertex {
    float pos[3];   // X, Y, Z
    float color[3]; // R, G, B
};

// struct ImageCreateInfo { ... };