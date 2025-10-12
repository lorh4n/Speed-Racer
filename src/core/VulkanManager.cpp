#include <core/VulkanManager.hpp>

VulkanManager::VulkanManager(int width, int height, const char *title) :
    window(width, height, title),
    instance(VK_NULL_HANDLE),
    surface(VK_NULL_HANDLE),
    debugMessenger(VK_NULL_HANDLE),
    physicalDevice(VK_NULL_HANDLE),
    device(VK_NULL_HANDLE),
    swapchainManager(nullptr) {
}

VulkanManager::~VulkanManager() {
	cleanup();
}

void VulkanManager::createSurface() {
	window.createSurface(instance, &surface);
}

// Executa a configuração completa da stack Vulkan respeitando as dependências entre etapas.
void VulkanManager::initVulkan() {
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	setupSwapChain();
	createGraphicsPipeline();
}

void VulkanManager::createGraphicsPipeline() {
	PipelineConfig pipelineConfig{};
	pipelineConfig.extend = swapchainManager->getSwapchainExtent();
	renderPass = pipelineManager.createRenderPass(device, swapchainManager->getSwapchainImageFormat());
	pipelineConfig.renderPass = renderPass;

	std::tie(graphicsPipeline, graphicsPipelineLayout) = pipelineManager.createBasicGraphicsPipeline(device, pipelineConfig);
}

void VulkanManager::createLogicalDevice() {
	// A fábrica retorna o dispositivo lógico juntamente com as filas configuradas.
	std::tie(device, queues) = LogicalDeviceCreator::create(
	    physicalDevice, queueManager, VulkanTools::enableValidationLayers, VulkanTools::validationLayers, deviceExtensions); // Using deviceExtensions from SwapchainManager.hpp
}

void VulkanManager::setupSwapChain() {
	// Cria o swapchain apenas após garantir que instância, dispositivo e superfícies estão prontos.
	swapchainManager = std::make_unique<SwapchainManager>(
	    device, physicalDevice, surface, *window.getWindow(), queueManager);
	swapchainManager->createSwapchain(window.getWidth(), window.getHeight());
	swapchainManager->createImageViews();
}

void VulkanManager::pickPhysicalDevice() {
	physicalDevice = PhysicalDeviceSelector::select(instance, surface, queueManager);
}

void VulkanManager::createInstance() {
	// Check validation layer support if enabled
	if (VulkanTools::enableValidationLayers && !VulkanTools::checkValidationLayerSupport()) {
		throw std::runtime_error("[VulkanManager] : Validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName   = "Speed Racer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName        = "No Engine";
	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Use VulkanTools::getRequiredExtensions() to get all required extensions
	auto extensions                    = VulkanTools::getRequiredExtensions();
	createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (VulkanTools::enableValidationLayers) {
		createInfo.enabledLayerCount   = static_cast<uint32_t>(VulkanTools::validationLayers.size());
		createInfo.ppEnabledLayerNames = VulkanTools::validationLayers.data();

		VulkanTools::populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount   = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("[VulkanManager] : Failed to create Vulkan instance");
	}
}

void VulkanManager::setupDebugMessenger() {
	if (VulkanTools::enableValidationLayers) {
		VulkanTools::setupDebugMessenger(instance, debugMessenger);
	}
	else {
		std::cout << "[VulkanManager] : Validation layers disabled, skipping debug messenger setup" << std::endl;
	}
}

void VulkanManager::mainLoop() {
	while (!window.shouldClose()) {
		window.pollEvents();
		// Add rendering logic here
	}
}

void VulkanManager::cleanup() {
	// Desaloca em ordem inversa de criação para evitar o uso de recursos já destruídos.
    if (graphicsPipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
    }
    if (graphicsPipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);
    }
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, renderPass, nullptr);
    }

	swapchainManager.reset();

	if (device != VK_NULL_HANDLE) {
		vkDestroyDevice(device, nullptr);
		device = VK_NULL_HANDLE;
	}
	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
		surface = VK_NULL_HANDLE;
	}
	if (VulkanTools::enableValidationLayers && debugMessenger != VK_NULL_HANDLE) {
		VulkanTools::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		debugMessenger = VK_NULL_HANDLE;
	}
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, nullptr);
		instance = VK_NULL_HANDLE;
	}

}

void VulkanManager::run() {
	initVulkan();
	mainLoop();
	cleanup();
}
