#include "core/BufferManager.hpp"

BufferManager::BufferManager(VkDevice         device,
                             VmaAllocator     allocator,
                             ResourceManager &resources,
                             CommandManager  &commands,
                             QueueManager    &queueManager) : device(device),
                                                           allocator(allocator),
                                                           resources(resources),
                                                           commands(commands),
                                                           queueManager(queueManager) {
}

BufferHandle BufferManager::createStagingBuffer(size_t size) {

	BufferHandle stagingBuffer = resources.createBuffer({ // "Designated Initializers" (do C++20).
		.size = size,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU
	});

	return stagingBuffer;
}