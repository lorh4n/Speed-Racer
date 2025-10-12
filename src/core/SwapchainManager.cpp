#include <core/SwapchainManager.hpp>

bool checkDeviceSupportSwapChain(VkPhysicalDevice device) {
	return true;
}

SwapchainManager::SwapChainSupportDetails SwapchainManager::querySwapchainSupport() {
	SwapchainManager::SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool SwapchainManager::checkDeviceSupportSwapChain(VkPhysicalDevice device) {
	std::cout << "[SwapchainManager] : Verifying if Device Supports Swap Chain";
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	if (requiredExtensions.empty()) {
		std::cout << ": OK" << std::endl;
	}
	else {
		std::cout << ": ERROR" << std::endl;
	}

	return requiredExtensions.empty();
}

bool SwapchainManager::createSwapchain(uint32_t width, uint32_t height) {
	SwapChainSupportDetails swapChainSupport = SwapchainManager::querySwapchainSupport();

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D         extent        = chooseSwapExtent(swapChainSupport.capabilities, width, height);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface          = surface;
	createInfo.minImageCount    = imageCount;
	createInfo.imageFormat      = surfaceFormat.format;
	createInfo.imageColorSpace  = surfaceFormat.colorSpace;
	createInfo.imageExtent      = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	this->swapchainImageFormat = surfaceFormat.format;
	this->swapchainExtent = extent;

	auto     families             = queueManager.getQueueFamilies();            // Pega o mapa de queue families
	uint32_t graphicsFamilyIndex  = families[QueueType::GRAPHICS].index;        // Índice da fila de gráficos
	uint32_t presentFamilyIndex   = families[QueueType::PRESENT].index;         // Índice da fila de apresentação
	uint32_t queueFamilyIndices[] = {graphicsFamilyIndex, presentFamilyIndex};

	if (graphicsFamilyIndex == presentFamilyIndex) {
		createInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;              // Optional
		createInfo.pQueueFamilyIndices   = nullptr;        // Optional
	}
	else {
		createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices   = queueFamilyIndices;
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

	// Especifica como lidar com a transparência. OPAQUE significa que a janela não será transparente.
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// Define o modo de apresentação que escolhemos anteriormente.
	createInfo.presentMode = presentMode;

	// Ativa o clipping, o que significa que não nos importamos com pixels que estão escondidos por outras janelas.
	createInfo.clipped = VK_TRUE;

	// Como esta é a nossa primeira swapchain, não há uma "antiga" para referenciar.
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("[SwapchainManager] : failed to create swap chain!");
	}

	return true;
}

VkSurfaceFormatKHR SwapchainManager::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
	for (const auto &availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR SwapchainManager::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
	for (const auto &availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapchainManager::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, uint32_t width, uint32_t height) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		// Get actual framebuffer size from GLFW
		int framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize(&window, &framebufferWidth, &framebufferHeight);

		VkExtent2D actualExtent = {static_cast<uint32_t>(framebufferWidth), static_cast<uint32_t>(framebufferHeight)};

		actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

bool SwapchainManager::createImageViews() {
   // --- O seu código está perfeito até aqui ---
   uint32_t imageCount; // Use uint32_t que é o tipo padrão do Vulkan
   vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);

   if (imageCount == 0) {
      throw std::runtime_error("[SwapchainManager] : Swapchain returned no images!");
   }
   
   swapchainImages.resize(imageCount);
   vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
   
   // --- Fim do seu código ---


   // 1. Redimensiona o vetor de ImageViews para ter o mesmo tamanho que o de Images.
   swapchainImageViews.resize(swapchainImages.size());

   // 2. Itera por cada VkImage para criar uma VkImageView correspondente.
   for (size_t i = 0; i < swapchainImages.size(); i++) {
      VkImageViewCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      createInfo.image = swapchainImages[i]; // A imagem em bruto que queremos "ver"

      // Descreve como a imagem será interpretada
      createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      createInfo.format = swapchainImageFormat; // O formato que guardámos!

      // Mapeamento de cores (padrão é não mudar nada - identidade)
      createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

      // Descreve qual a parte da imagem que será acedida
      createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      createInfo.subresourceRange.baseMipLevel = 0;
      createInfo.subresourceRange.levelCount = 1;
      createInfo.subresourceRange.baseArrayLayer = 0;
      createInfo.subresourceRange.layerCount = 1;

      // 3. Cria a ImageView
      if (vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS) {
         throw std::runtime_error("[SwapchainManager] : Failed to create swap chain image views!");
      }
   }

   std::cout << "[SwapchainManager] : Image views created successfully!" << std::endl;
   return true;
}

void SwapchainManager::cleanup() {

	for (auto framebuffer : swapchainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	for (auto imageView : swapchainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    // --------------------------

	if (swapchain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device, swapchain, nullptr);
	}
}


// ================== FrameBuffers ============================

bool SwapchainManager::createFramebuffers(VkRenderPass renderPass) {
	// Redimensiona o vetor para ter o mesmo tamanho que imageViews
	swapchainFramebuffers.resize(swapchainImageViews.size());

	// Cria um framebuffer para cada imageView
	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapchainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		if(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("[SwapchainManager] : Failed to create framebuffer!");
		}
	}
	std::cout << "[SwapchainManager] : Framebuffers created successfully! (" << swapchainFramebuffers.size() << " framebuffers)" << std::endl;
   return true;


}