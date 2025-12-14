#include "core/VmaWrapper.hpp"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

VmaWrapper::VmaWrapper() : allocator(VK_NULL_HANDLE), device(VK_NULL_HANDLE) {
	std::cout << "[VmaWrapper]:\t VmaWrapper created (uninitialized)" << std::endl;
}

void VmaWrapper::initialize(VkDevice device, VkPhysicalDevice physicalDevice, VkInstance instance) {
	if (isInitialized()) {
		throw std::runtime_error("[VmaWrapper]:\t Already initialized!");
	}

	VmaAllocatorCreateInfo allocInfo{};
	allocInfo.device         = device;
	allocInfo.physicalDevice = physicalDevice;
	allocInfo.instance       = instance;
	// Resto usa valores padrão (flags = 0, ponteiros = nullptr)

	VkResult result = vmaCreateAllocator(&allocInfo, &allocator);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("[VmaWrapper]: \t Failed to initialize VMA!");
	}

	this->device = device;

	std::cout << "[VmaWrapper]: \t VMA initialized successfully!" << std::endl;
}

VmaWrapper::~VmaWrapper() {
	// TODO: O que fazer aqui?

	// Dica 1: Você precisa chamar vmaDestroyAllocator

	// Dica 2: E se initialize() nunca foi chamado?

	if (isInitialized()) {
		vmaDestroyAllocator(allocator);

		std::cout << "[VmaWrapper]: \t Allocator destroyed." << std::endl;
	}
}

VmaBuffer VmaWrapper::createBuffer(const VkBufferCreateInfo &bufferInfo, const VmaAllocationCreateInfo &allocInfo) {
	VmaBuffer vmaBuffer;

	if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &vmaBuffer.buffer, &vmaBuffer.allocation, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("[VmaWrapper]: Failed to create buffer!");
	}

	return vmaBuffer;
}

void VmaWrapper::destroyBuffer(VmaBuffer &buffer) {
	if (buffer.buffer != VK_NULL_HANDLE) {
		vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);

		buffer.buffer = VK_NULL_HANDLE;

		buffer.allocation = VK_NULL_HANDLE;
	}
}
