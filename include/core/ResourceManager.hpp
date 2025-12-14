#pragma once

#include <core/Handle.hpp>
#include <core/ResourceTypes.hpp>
#include <core/VmaWrapper.hpp>

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <stdexcept> 


class ResourceManager {
public:
   ResourceManager(VkDevice device, VmaAllocator allocator, VmaWrapper& vmaWrapper);
   ~ResourceManager();

   ResourceManager(const ResourceManager&) = delete;
   ResourceManager& operator = (const ResourceManager&) = delete;

   BufferHandle createBuffer(const BufferCreateInfo& info);
   void destroyBuffer(BufferHandle handle);
   VmaBuffer getBuffer(BufferHandle handle) const; // Alterado para retornar VmaBuffer para mais informações
   VkBuffer getVkBuffer(BufferHandle handle) const;
private:
   VkDevice m_device;
   VmaAllocator m_allocator;
   VmaWrapper* m_vmaWrapper;

   std::unordered_map<BufferHandle, VmaBuffer> m_buffers;

   HandleAllocator<BufferHandle> m_bufferHandleAllocator;

   // HandleAllocator<ImageHandle> m_imageHandleAllocator;
};