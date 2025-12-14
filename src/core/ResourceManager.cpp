#include <core/ResourceManager.hpp>


ResourceManager::ResourceManager(VkDevice device, VmaAllocator allocator, VmaWrapper& vmaWrapper) 
    :   m_device(device), 
        m_allocator(allocator), 
        m_vmaWrapper(&vmaWrapper) {}
ResourceManager::~ResourceManager() {
   for (auto& [handle, buffer] : m_buffers) {
      m_vmaWrapper->destroyBuffer(buffer);
   }
   m_buffers.clear();
};

BufferHandle ResourceManager::createBuffer(const BufferCreateInfo& info) {
   VkBufferCreateInfo bufferInfo{};
   bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
   bufferInfo.size = info.size;
   bufferInfo.usage = info.usage;

   VmaAllocationCreateInfo allocInfo{};
   allocInfo.usage = info.memoryUsage;

   VmaBuffer newVmaBuffer = m_vmaWrapper->createBuffer(bufferInfo, allocInfo);

   BufferHandle handle = m_bufferHandleAllocator.allocate();

   m_buffers[handle] = newVmaBuffer;

   return handle;
}


void ResourceManager::destroyBuffer(BufferHandle handle) {
    // Procura o buffer no mapa.
    auto it = m_buffers.find(handle);
    if (it != m_buffers.end()) {
        // Se encontrou, chama a VMA para destruir o buffer e liberar a memória.
        vmaDestroyBuffer(m_allocator, it->second.buffer, it->second.allocation);
        // Remove o buffer do mapa.
        m_buffers.erase(it);
    }
}
VmaBuffer ResourceManager::getBuffer(BufferHandle handle) const {
    auto it = m_buffers.find(handle);
    if (it != m_buffers.end()) {
        return it->second;
    }
    // Retorna um buffer "vazio" se o handle não for encontrado.
    return {VK_NULL_HANDLE, VK_NULL_HANDLE};
}

VkBuffer ResourceManager::getVkBuffer(BufferHandle handle) const {
    return getBuffer(handle).buffer;
}