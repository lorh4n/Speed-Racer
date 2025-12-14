#ifndef BUFFER_MANAGER_HPP
#define BUFFER_MANAGER_HPP

#include <vulkan/vulkan.h>

#include <core/CommandManager.hpp>
#include <core/Handle.hpp>
#include <functional>
#include <cstring> // Para memcpy
#include <core/ResourceManager.hpp>
#include <core/ResourceTypes.hpp>
#include <core/queueManager.hpp>


class BufferManager {
  public:
	BufferManager(VkDevice         device,
	              VmaAllocator     allocator,
	              ResourceManager &resources,
	              CommandManager  &commands,
	              QueueManager    &queueManager);
	~BufferManager();

	BufferManager(const BufferManager &)            = delete;
	BufferManager &operator=(const BufferManager &) = delete;

	BufferHandle createVertexBuffer(const void *data, size_t size);
	BufferHandle createIndexBuffer(const void *data, size_t size);
	BufferHandle createUniformBuffer(size_t size);
	BufferHandle createStagingBuffer(size_t size);

	void uploadToBuffer(BufferHandle dtsBuffer, const void *data, size_t size);
	void copyBuffer(BufferHandle srcBuffer,
	                BufferHandle dstBuffer,
	                VkDeviceSize size,
	                VkDeviceSize srcOffset = 0,
	                VkDeviceSize dstOffset = 0);

	// Memory mapping
	void *mapBuffer(BufferHandle buffer);
	void  unmapBuffer(BufferHandle buffer);
	void  updateBuffer(BufferHandle buffer, const void *data, size_t size);

	// Getters
	VkBuffer getVkBuffer(BufferHandle handle) const {
		return resources.getVkBuffer(handle);
	}

  private:
	VkDevice         device;
	VmaAllocator     allocator;
	ResourceManager &resources;
	CommandManager  &commands;
	QueueManager    &queueManager;

	void executeOneTimeCommands(std::function<void(VkCommandBuffer)> cmdFunc);
};

#endif