#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h" 
#include <vector> 


struct BufferCreateInfo {
   VkDeviceSize size;
   VkBufferUsageFlags usage;
   VmaMemoryUsage memoryUsage;
};

struct Vertex {
    float pos[3];   // X, Y, Z
    float color[3]; // R, G, B
};

// Dados brutos da mesh (CPU side)
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

// struct ImageCreateInfo { ... };