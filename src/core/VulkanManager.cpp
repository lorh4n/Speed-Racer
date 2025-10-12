#include <core/VulkanManager.hpp>
#include <core/RenderPassManager.hpp>

VulkanManager::VulkanManager(int width, int height, const char *title) :
    window(width, height, title),
    instance(VK_NULL_HANDLE),
    surface(VK_NULL_HANDLE),
    debugMessenger(VK_NULL_HANDLE),
    physicalDevice(VK_NULL_HANDLE),
    device(VK_NULL_HANDLE),
    swapchainManager(nullptr) {
    std::cout << "[VulkanManager] : VulkanManager created." << std::endl;
}

VulkanManager::~VulkanManager() {
    std::cout << "[VulkanManager] : VulkanManager destructor called." << std::endl;
	cleanup();
}

void VulkanManager::createSurface() {
	window.createSurface(instance, &surface);
    std::cout << "[VulkanManager] : Surface created." << std::endl;
}

// Executa a configuração completa da stack Vulkan respeitando as dependências entre etapas.
void VulkanManager::initVulkan() {
    std::cout << "[VulkanManager] : Initializing Vulkan..." << std::endl;
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	setupSwapChain();
	createGraphicsPipeline();
	createFramebuffers();
    std::cout << "[VulkanManager] : Vulkan initialized successfully." << std::endl;
}

void VulkanManager::createFramebuffers() {
	swapchainManager->createFramebuffers(renderPass);
	std::cout << "[VulkanManager] : Framebuffers created." << std::endl;
}

void VulkanManager::createGraphicsPipeline() {
	PipelineConfig pipelineConfig{};
	pipelineConfig.extend = swapchainManager->getSwapchainExtent();
	renderPass = RenderPassManager::createBasicRenderPass(device, swapchainManager->getSwapchainImageFormat());
	pipelineConfig.renderPass = renderPass;
    std::cout << "[VulkanManager] : RenderPass created." << std::endl;

	std::tie(graphicsPipeline, graphicsPipelineLayout) = PipelineManager::createGraphicsPipeline(device, pipelineConfig);
    std::cout << "[VulkanManager] : Graphics pipeline created." << std::endl;
}

void VulkanManager::createLogicalDevice() {
	// A fábrica retorna o dispositivo lógico juntamente com as filas configuradas.
	std::tie(device, queues) = LogicalDeviceCreator::create(
	    physicalDevice, queueManager, VulkanTools::enableValidationLayers, VulkanTools::validationLayers, deviceExtensions); // Using deviceExtensions from SwapchainManager.hpp
    std::cout << "[VulkanManager] : Logical device created." << std::endl;
}

void VulkanManager::setupSwapChain() {
	// Cria o swapchain apenas após garantir que instância, dispositivo e superfícies estão prontos.
	swapchainManager = std::make_unique<SwapchainManager>(
	    device, physicalDevice, surface, *window.getWindow(), queueManager);
	swapchainManager->createSwapchain(window.getWidth(), window.getHeight());
	swapchainManager->createImageViews();
    std::cout << "[VulkanManager] : Swapchain setup complete." << std::endl;
}

void VulkanManager::pickPhysicalDevice() {
	physicalDevice = PhysicalDeviceSelector::select(instance, surface, queueManager);
    std::cout << "[VulkanManager] : Physical device selected." << std::endl;
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
    std::cout << "[VulkanManager] : Vulkan instance created." << std::endl;
}

void VulkanManager::setupDebugMessenger() {
	if (VulkanTools::enableValidationLayers) {
		VulkanTools::setupDebugMessenger(instance, debugMessenger);
        std::cout << "[VulkanManager] : Debug messenger setup." << std::endl;
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
    std::cout << "[VulkanManager] : Starting cleanup..." << std::endl;

	// Desaloca em ordem inversa de criação para evitar o uso de recursos já destruídos.
    if (graphicsPipeline != VK_NULL_HANDLE || graphicsPipelineLayout != VK_NULL_HANDLE) {
        PipelineManager::destroy(device, graphicsPipeline, graphicsPipelineLayout);
    }

    // O Swapchain e seus framebuffers dependem do RenderPass, então devem ser destruídos antes.
    swapchainManager.reset();
    std::cout << "[VulkanManager] : Swapchain manager destroyed." << std::endl;

    if (renderPass != VK_NULL_HANDLE) {
        RenderPassManager::destroy(device, renderPass);
        std::cout << "[VulkanManager] : Render pass destroyed." << std::endl;
    }

	if (device != VK_NULL_HANDLE) {
		vkDestroyDevice(device, nullptr);
        std::cout << "[VulkanManager] : Logical device destroyed." << std::endl;
		device = VK_NULL_HANDLE;
	}
	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
        std::cout << "[VulkanManager] : Surface destroyed." << std::endl;
		surface = VK_NULL_HANDLE;
	}
	if (VulkanTools::enableValidationLayers && debugMessenger != VK_NULL_HANDLE) {
		VulkanTools::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        std::cout << "[VulkanManager] : Debug messenger destroyed." << std::endl;
		debugMessenger = VK_NULL_HANDLE;
	}
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, nullptr);
        std::cout << "[VulkanManager] : Vulkan instance destroyed." << std::endl;
		instance = VK_NULL_HANDLE;
	}

    std::cout << "[VulkanManager] : Cleanup complete." << std::endl;
}

void VulkanManager::run() {
	initVulkan();
	mainLoop();
	cleanup();
}
