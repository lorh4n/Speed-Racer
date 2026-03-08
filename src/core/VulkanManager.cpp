#include <chrono>
#include <core/RenderPassManager.hpp>
#include <core/VulkanManager.hpp>
#include <glm/gtc/matrix_transform.hpp>

VulkanManager::VulkanManager(int width, int height, const char *title) :
    window(width, height, title),
    instance(VK_NULL_HANDLE),
    surface(VK_NULL_HANDLE),
    debugMessenger(VK_NULL_HANDLE),
    physicalDevice(VK_NULL_HANDLE),
    device(VK_NULL_HANDLE),
    swapchainManager(nullptr),
    commandManager(nullptr),
    framebufferResized(false),
    cubeMesh(nullptr),
    triangleMesh(nullptr) {
	std::cout << "[VulkanManager] : VulkanManager created." << std::endl;

	// Set user pointer so callback can access this instance
	glfwSetWindowUserPointer(window.getWindow(), this);

	// Set framebuffer resize callback
	window.setFramebufferResizeCallback(framebufferResizeCallback);
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
	setupVmaWrapper();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();
	createResourceManager();
	createBufferManager();

	createCube();
	createTriangle();

	std::cout << "[VulkanManager] : Vulkan initialized successfully." << std::endl;
}

void VulkanManager::createBufferManager() {
	bufferManager = std::make_unique<BufferManager>(
	    device,
	    vmaWrapper.getAllocator(),
	    *resourceManager,
	    *commandManager,
	    queueManager);
	std::cout << "[VulkanManager] : Buffer Manager initialized." << std::endl;
}

void VulkanManager::createResourceManager() {
	resourceManager = std::make_unique<ResourceManager>(
	    device,
	    vmaWrapper.getAllocator(),
	    vmaWrapper);
	std::cout << "[VulkanManager] : Resource Manager initialized." << std::endl;
}

void VulkanManager::setupVmaWrapper() {
	vmaWrapper.initialize(device, physicalDevice, instance);
}

void VulkanManager::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
	// Retrieve the VulkanManager instance from the window user pointer
	auto app = reinterpret_cast<VulkanManager *>(glfwGetWindowUserPointer(window));

	if (app) {
		app->framebufferResized = true;
		std::cout << "[VulkanManager] : Framebuffer resized to " << width << "x" << height << std::endl;
	}
}

void VulkanManager::recreateSwapChain() {
	std::cout << "[VulkanManager] : Recreating swap chain..." << std::endl;

	int width  = 0;
	int height = 0;
	glfwGetFramebufferSize(window.getWindow(), &width, &height);

	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window.getWindow(), &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	swapchainManager->recreateSwapchain(width, height);

	createFramebuffers();

	std::cout << "[VulkanManager] : Swap chain recreation complete." << std::endl;
}

void VulkanManager::drawFrame() {
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(
	    device,
	    swapchainManager->getSwapchain(),
	    UINT64_MAX,
	    imageAvailableSemaphores[currentFrame],
	    VK_NULL_HANDLE,
	    &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		std::cout << "[VulkanManager] : Swapchain out of date, recreating..." << std::endl;
		recreateSwapChain();
		return;        // Try again next frame
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("[VulkanManager] : Failed to acquire swap chain image!");
	}

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore          waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount         = 1;
	submitInfo.pWaitSemaphores            = waitSemaphores;
	submitInfo.pWaitDstStageMask          = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[]  = {renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores    = signalSemaphores;

	if (vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores    = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapchainManager->getSwapchain()};
	presentInfo.swapchainCount  = 1;
	presentInfo.pSwapchains     = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(queues.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		std::cout << "[VulkanManager] : Swapchain suboptimal or resized, recreating..." << std::endl;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("[VulkanManager] : Failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanManager::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;        // Importante! Começa sinalizado

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
		    vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
		    vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

	std::cout << "[VulkanManager] : Synchronization objects created." << std::endl;
}

void VulkanManager::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags            = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("[VulkanManager] : Failed to begin recording command buffer!");
	}

	// Começar RenderPass

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass        = renderPass;
	renderPassInfo.framebuffer       = swapchainManager->getFramebuffers()[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = swapchainManager->getSwapchainExtent();

	VkClearValue clearColor        = {{{0.2f, 0.2f, 0.2f, 1.0f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues    = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Bind Pipeline
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	// Configurar viewport e scissor (dinâmicos)
	VkViewport viewport{};
	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = static_cast<float>(swapchainManager->getSwapchainExtent().width);
	viewport.height   = static_cast<float>(swapchainManager->getSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapchainManager->getSwapchainExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// --- CÁLCULO DE TEMPO ---
	static auto startTime   = std::chrono::high_resolution_clock::now();
	auto        currentTime = std::chrono::high_resolution_clock::now();
	float       time        = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	// Matrizes fixas (Câmera e Projeção)
	glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), swapchainManager->getSwapchainExtent().width / (float) swapchainManager->getSwapchainExtent().height, 0.1f, 10.0f);
	proj[1][1] *= -1;        // Correção do Y invertido do Vulkan

	MeshPushConstants constants;

	// --- DESENHAR O CUBO (À DIREITA) ---
	if (cubeMesh) {
		cubeMesh->bind(commandBuffer);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.0f, 0.0f));
		model           = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		constants.render_matrix = proj * view * model;

		vkCmdPushConstants(commandBuffer, graphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);
		cubeMesh->draw(commandBuffer);
	}

	// --- DESENHAR O TRIÂNGULO (À ESQUERDA) ---
	if (triangleMesh) {
		triangleMesh->bind(commandBuffer);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.8f, 0.0f, 0.0f));
		model           = glm::rotate(model, time * glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		constants.render_matrix = proj * view * model;

		vkCmdPushConstants(commandBuffer, graphicsPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);
		triangleMesh->draw(commandBuffer);
	}

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("[VulkanManager] : Failed to record command buffer!");
	}
}

void VulkanManager::createCommandPool() {
	commandManager = std::make_unique<CommandManager>(device, queueManager);
	commandManager->createCommandPool();
	std::cout << "[VulkanManager] : Command pool setup complete." << std::endl;
}

void VulkanManager::createCommandBuffers() {
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	commandBuffers = commandManager->allocateCommandBuffers(MAX_FRAMES_IN_FLIGHT);
	std::cout << "[VulkanManager] : Command buffers created." << std::endl;
	// NÃO gravar aqui - será feito no drawFrame()
}

void VulkanManager::createFramebuffers() {
	swapchainManager->createFramebuffers(renderPass);
	std::cout << "[VulkanManager] : Framebuffers created." << std::endl;
}

void VulkanManager::createGraphicsPipeline() {
	PipelineConfig pipelineConfig{};
	pipelineConfig.extend     = swapchainManager->getSwapchainExtent();
	renderPass                = RenderPassManager::createBasicRenderPass(device, swapchainManager->getSwapchainImageFormat());
	pipelineConfig.renderPass = renderPass;
	std::cout << "[VulkanManager] : RenderPass created." << std::endl;

	std::tie(graphicsPipeline, graphicsPipelineLayout) = PipelineManager::createGraphicsPipeline(device, pipelineConfig);
	std::cout << "[VulkanManager] : Graphics pipeline created." << std::endl;
}

void VulkanManager::createLogicalDevice() {
	// A fábrica retorna o dispositivo lógico juntamente com as filas configuradas.
	std::tie(device, queues) = LogicalDeviceCreator::create(
	    physicalDevice, queueManager, VulkanTools::enableValidationLayers, VulkanTools::validationLayers, deviceExtensions);        // Using deviceExtensions from SwapchainManager.hpp
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
	bool useValidationLayers = VulkanTools::enableValidationLayers;

	// Check validation layer support and disable if not available
	if (useValidationLayers && !VulkanTools::checkValidationLayerSupport()) {
		std::cerr << "[VulkanManager] : Validation layers requested but NOT available on this system. Proceeding without them." << std::endl;
		useValidationLayers = false;
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
	if (useValidationLayers) {
		createInfo.enabledLayerCount   = static_cast<uint32_t>(VulkanTools::validationLayers.size());
		createInfo.ppEnabledLayerNames = VulkanTools::validationLayers.data();

		VulkanTools::populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount   = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.pNext               = nullptr;
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
	std::cout << "[VulkanManager] : Entering main loop..." << std::endl;
	while (!window.shouldClose()) {
		window.pollEvents();
		drawFrame();
		// Add rendering logic here
	}
	std::cout << "[VulkanManager] : Exiting main loop." << std::endl;
}

void VulkanManager::cleanup() {
	std::cout << "[VulkanManager] : Starting cleanup..." << std::endl;

	if (device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(device);
	}

	cubeMesh.reset();
	triangleMesh.reset();

	bufferManager.reset();
	resourceManager.reset();

	// Destruir o VMA Allocator ANTES do VkDevice
	vmaWrapper.destroy();

	// Apenas destruir objetos de sincronização se eles foram criados
	if (device != VK_NULL_HANDLE && !renderFinishedSemaphores.empty()) {
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (renderFinishedSemaphores[i] != VK_NULL_HANDLE)
				vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			if (imageAvailableSemaphores[i] != VK_NULL_HANDLE)
				vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			if (inFlightFences[i] != VK_NULL_HANDLE)
				vkDestroyFence(device, inFlightFences[i], nullptr);
		}
		std::cout << "[VulkanManager] : Synchronization objects destroyed." << std::endl;
	}

	// Command manager limpa automaticamente o pool e buffers
	commandManager.reset();
	std::cout << "[VulkanManager] : Command manager destroyed." << std::endl;

	// Desaloca em ordem inversa de criação para evitar o uso de recursos já destruídos.
	if (device != VK_NULL_HANDLE && (graphicsPipeline != VK_NULL_HANDLE || graphicsPipelineLayout != VK_NULL_HANDLE)) {
		PipelineManager::destroy(device, graphicsPipeline, graphicsPipelineLayout);
	}

	// O Swapchain e seus framebuffers dependem do RenderPass, então devem ser destruídos antes.
	swapchainManager.reset();
	std::cout << "[VulkanManager] : Swapchain manager destroyed." << std::endl;

	if (device != VK_NULL_HANDLE && renderPass != VK_NULL_HANDLE) {
		RenderPassManager::destroy(device, renderPass);
		std::cout << "[VulkanManager] : Render pass destroyed." << std::endl;
	}
	if (device != VK_NULL_HANDLE) {
		vkDestroyDevice(device, nullptr);
		std::cout << "[VulkanManager] : Logical device destroyed." << std::endl;
		device = VK_NULL_HANDLE;
	}
	if (instance != VK_NULL_HANDLE) {
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
		vkDestroyInstance(instance, nullptr);
		std::cout << "[VulkanManager] : Vulkan instance destroyed." << std::endl;
		instance = VK_NULL_HANDLE;
	}

	std::cout << "[VulkanManager] : Cleanup complete." << std::endl;
}

void VulkanManager::run() {
	initVulkan();
	mainLoop();
	// cleanup(); // Removido para evitar dupla liberação. O destrutor cuidará disso.
}

void VulkanManager::createCube() {
	cubeMesh = std::make_unique<Mesh>(bufferManager.get());
	cubeMesh->upload(MeshFactory::makeCube(), *bufferManager);
	std::cout << "[VulkanManager] : Cube mesh created." << std::endl;
}

void VulkanManager::createTriangle() {
	triangleMesh = std::make_unique<Mesh>(bufferManager.get());
	triangleMesh->upload(MeshFactory::makeTriangle(), *bufferManager);
	std::cout << "[VulkanManager] : Triangle mesh created." << std::endl;
}
