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

void VmaWrapper::destroy() {
	if (isInitialized()) {
		vmaDestroyAllocator(allocator);
		allocator = VK_NULL_HANDLE; // Evita dupla liberação
		std::cout << "[VmaWrapper]: \t Allocator destroyed." << std::endl;
	}
}

VmaWrapper::~VmaWrapper() {
	// A destruição agora é gerenciada por destroy() para garantir a ordem correta
	// em relação à destruição do VkDevice.
	// No entanto, ainda podemos manter uma verificação aqui como segurança.
	if (isInitialized()) {
		// Idealmente, destroy() já foi chamado.
		// Logar um aviso se o allocator ainda existir pode ser útil para debug.
		std::cout << "[VmaWrapper]: \t Destructor called, but allocator was not explicitly destroyed. Cleaning up now." << std::endl;
		destroy();
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
