#include <core/ScopedBuffer.hpp>

ScopedBuffer::ScopedBuffer(VmaAllocator allocator, VmaBuffer bufferData) : 
   allocator_(allocator),                                                              
   buffer_(bufferData.buffer),                                                               
   allocation_(bufferData.allocation) 
{
   std::cout << "[ScopedBuffer] Taking ownership of buffer" << std::endl;
}

ScopedBuffer::~ScopedBuffer(){
   if (allocator_ && buffer_ != VK_NULL_HANDLE && allocation_ != nullptr) {
      vmaDestroyBuffer(allocator_, buffer_, allocation_);
      std::cout << "[ScopedBuffer] Buffer automatically destroyed" << std::endl;
      buffer_ = VK_NULL_HANDLE;
      allocation_ = VK_NULL_HANDLE;
   }
}
