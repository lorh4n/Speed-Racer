#ifndef SCOPED_BUFFER_HPP
#define SCOPED_BUFFER_HPP

#include <core/VmaWrapper.hpp>
#include <vulkan/vulkan.h>

class ScopedBuffer {
public:
    // Constructor: takes ownership of an existing buffer
    ScopedBuffer(VmaAllocator allocator, VmaBuffer bufferData);
    
    // Destructor: RAII cleanup
    ~ScopedBuffer();
    
    // Delete copy (no sharing ownership)
    ScopedBuffer(const ScopedBuffer&) = delete;
    ScopedBuffer& operator=(const ScopedBuffer&) = delete;
    
    // Move semantics (transfer ownership)
    ScopedBuffer(ScopedBuffer&& other) noexcept;
    ScopedBuffer& operator=(ScopedBuffer&& other) noexcept;
    
    // Getters
    VkBuffer get() const { return buffer_; }
    VmaAllocation getAllocation() const { return allocation_; }
    
    // Optional: explicit check if valid
    bool isValid() const { return buffer_ != VK_NULL_HANDLE; }

private:
    VmaAllocator allocator_;   // Needed for cleanup
    VkBuffer buffer_;          // The actual Vulkan buffer
    VmaAllocation allocation_; // VMA's memory allocation handle
};

#endif