#pragma once

#include <cstdint>
#include <limits>
#include <vector>


using BufferHandle = uint32_t;
using ImageHadle = uint32_t; // New way with using (before was Typedef)

constexpr uint32_t INVALID_HANDLE = std::numeric_limits<uint32_t>::max();

template <typename THandle>

class HandleAllocator {
public:
   HandleAllocator() {
      m_nextHandle = 0;
   }

   THandle allocate() {
      return m_nextHandle++;  
   }

   void free(THandle handle) {

   }

private: 
   uint32_t m_nextHandle;

};
