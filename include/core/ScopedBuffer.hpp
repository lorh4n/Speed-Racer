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
	ScopedBuffer(const ScopedBuffer &)            = delete;
	ScopedBuffer &operator=(const ScopedBuffer &) = delete;

	// Move semantics (transfer ownership)
	ScopedBuffer(ScopedBuffer &&other) noexcept
	    : allocator_(other.allocator_),
	      buffer_(other.buffer_),
	      allocation_(other.allocation_) {
		// Invalidate the source
		other.buffer_     = VK_NULL_HANDLE;
		other.allocation_ = VK_NULL_HANDLE;
	}
	ScopedBuffer& operator=(ScopedBuffer&& other) noexcept {
		if (this != &other) {
			// Destroy current buffer if exists
			if (buffer_ != VK_NULL_HANDLE) {
				vmaDestroyBuffer(allocator_, buffer_, allocation_);
			}
			
			// Transfer ownership
			allocator_ = other.allocator_;
			buffer_ = other.buffer_;
			allocation_ = other.allocation_;
			
			// Invalidate source
			other.buffer_ = VK_NULL_HANDLE;
			other.allocation_ = VK_NULL_HANDLE;
		}
		return *this;
	}

	// Getters
	VkBuffer get() const {
		return buffer_;
	}
	VmaAllocation getAllocation() const {
		return allocation_;
	}

	// Optional: explicit check if valid
	bool isValid() const {
		return buffer_ != VK_NULL_HANDLE;
	}

  private:
	VmaAllocator  allocator_;         // Needed for cleanup
	VkBuffer      buffer_;            // The actual Vulkan buffer
	VmaAllocation allocation_;        // VMA's memory allocation handle
};

#endif