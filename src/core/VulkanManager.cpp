#include <core/VulkanManager.hpp>

VulkanManager::VulkanManager(int width, int height, const char* title)
    : window(width, height, title),
      instance(VK_NULL_HANDLE),
      surface(VK_NULL_HANDLE),
      debugMessenger(VK_NULL_HANDLE),
      physicalDevice(VK_NULL_HANDLE),
      device(VK_NULL_HANDLE) {
}

VulkanManager::~VulkanManager() {
    cleanup();
}

void VulkanManager::initVulkan() {
    createInstance();
    setupDebugMessenger();
    window.createSurface(instance, &surface);
    pickPhysicalDevice();
    
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    
    std::tie(device, queues) = LogicalDeviceCreator::create(
        physicalDevice, queueManager, VulkanTools::enableValidationLayers, VulkanTools::validationLayers, deviceExtensions);
}

void VulkanManager::pickPhysicalDevice() {
    physicalDevice = PhysicalDeviceSelector::select(instance, surface, queueManager);
}

void VulkanManager::createInstance() {
    // Check validation layer support if enabled
    if (VulkanTools::enableValidationLayers && !VulkanTools::checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available!");
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = window.getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (VulkanTools::enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanTools::validationLayers.size());
        createInfo.ppEnabledLayerNames = VulkanTools::validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance");
    }
}

void VulkanManager::setupDebugMessenger() {
    if (VulkanTools::enableValidationLayers) {
        VulkanTools::setupDebugMessenger(instance, debugMessenger);
    }
}

void VulkanManager::mainLoop() {
    while (!window.shouldClose()) {
        window.pollEvents();
        // Add rendering logic here
    }
}

void VulkanManager::cleanup() {
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