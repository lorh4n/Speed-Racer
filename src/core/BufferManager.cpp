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

BufferManager::~BufferManager() {
}

BufferHandle BufferManager::createStagingBuffer(size_t size) {
	BufferHandle stagingBuffer = resources.createBuffer({// "Designated Initializers" (do C++20).
		.size        = size,
		.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		.memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU
	});

	return stagingBuffer;
}

BufferHandle BufferManager::createVertexBuffer(const void *data, size_t size) {
	BufferHandle stagingBuffer = createStagingBuffer(size);

	updateBuffer(stagingBuffer, data, size);

	BufferHandle vertexBuffer = resources.createBuffer(
	{// "Designated Initializers" (do C++20).
		.size        = size,
		.usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY
	});

	copyBuffer(stagingBuffer, vertexBuffer, size);

	resources.destroyBuffer(stagingBuffer);

	return vertexBuffer;
}

BufferHandle BufferManager::createIndexBuffer(const void *data, size_t size) {
   // Cria o Staging
   BufferHandle stagingBuffer = createStagingBuffer(size);

   // Carrega os dados no Staging
   updateBuffer(stagingBuffer, data, size);

   // Cria o buffer final na GPU (Agora com a flag de INDEX BUFFER)
   BufferHandle indexBuffer = resources.createBuffer({
       .size        = size,
       .usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
       .memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY
   });

   // Move do Staging para o final
   copyBuffer(stagingBuffer, indexBuffer, size);

   // Libera o Staging
   resources.destroyBuffer(stagingBuffer);

   return indexBuffer;
}

BufferHandle BufferManager::createUniformBuffer(size_t size) {
	BufferHandle uniformBuffer = resources.createBuffer({
			.size        = size,
			.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			.memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU 
		});

		return uniformBuffer;
}

void BufferManager::copyBuffer(BufferHandle srcHandle, BufferHandle dstHandle, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
	executeOneTimeCommands([&](VkCommandBuffer commandBuffer) {
		// Aqui dentro nós gravamos os comandos de cópia

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = srcOffset;
		copyRegion.dstOffset = dstOffset;
		copyRegion.size      = size;

		vkCmdCopyBuffer(
		    commandBuffer,
		    getVkBuffer(srcHandle),        // Pega o VkBuffer real do handle de origem
		    getVkBuffer(dstHandle),        // Pega o VkBuffer real do handle de destino
		    1,
		    &copyRegion);
	});
}

void BufferManager::executeOneTimeCommands(std::function<void(VkCommandBuffer)> cmdFunc) {
	// Alocar buffer temporario
	VkCommandBuffer commandBuffer = commands.allocateCommandBuffers(1)[0];

	// Começa Gravação
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	// Executa a função que contém seus comandos
	cmdFunc(commandBuffer);

	// Termina a gravação
	vkEndCommandBuffer(commandBuffer);

	// Envia para fila
	VkSubmitInfo submitInfo{};
	submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;

	// Precisamos da fila gráfica. O QueueManager nos ajuda aqui.
	VkQueue graphicsQueue = queueManager.getQueue(device, QueueType::GRAPHICS);
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

	// Espera a GPU terminar (importante para não deletar o buffer enquanto usa!)
	vkQueueWaitIdle(graphicsQueue);

	// Libera o command buffer
	vkFreeCommandBuffers(device, commands.getCommandPool(), 1, &commandBuffer);
}

// Memory Maping

void *BufferManager::mapBuffer(BufferHandle handle) {
	VmaBuffer buffer = resources.getBuffer(handle);
	void     *mappedData;
	vmaMapMemory(allocator, buffer.allocation, &mappedData);

	return mappedData;
}
void BufferManager::unmapBuffer(BufferHandle handle) {
	VmaBuffer buffer = resources.getBuffer(handle);
	vmaUnmapMemory(allocator, buffer.allocation);
}

void BufferManager::updateBuffer(BufferHandle buffer, const void *data, size_t size) {
	void *mappedData = mapBuffer(buffer);
	memcpy(mappedData, data, size);
	unmapBuffer(buffer);
}