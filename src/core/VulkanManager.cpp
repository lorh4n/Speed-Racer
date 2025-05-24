#include <core/VulkanManager.hpp>

VulkanManager::VulkanManager(int width, int height, const char *title) : window(width, height, title), instance(VK_NULL_HANDLE), surface(VK_NULL_HANDLE), debugMessenger(VK_NULL_HANDLE), physicalDevice (VK_NULL_HANDLE)
{}

void VulkanManager::run()
{
    initVulkan();
    mainLoop();
}

void VulkanManager::mainLoop()
{
    while (!window.shouldClose())
    {
        window.pollEvents();
    }
}

void VulkanManager::initVulkan()
{
    createInstance();
    VulkanTools::setupDebugMessenger(instance, debugMessenger);
    pickPhysicalDevice();
}

void VulkanManager::cleanup()
{
    if (VulkanTools::enableValidationLayers)
    {
        VulkanTools::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
}

VulkanManager::~VulkanManager() { cleanup(); }

void VulkanManager::createInstance()
{
    if (VulkanTools::enableValidationLayers && !VulkanTools::checkValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested, but not available!");
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Speed Racer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "RacerEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = VulkanTools::getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (VulkanTools::enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanTools::validationLayers.size());
        createInfo.ppEnabledLayerNames = VulkanTools::validationLayers.data();
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = VulkanTools::debugCallback;
        debugCreateInfo.pUserData = nullptr;
        createInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }
}

void VulkanManager::pickPhysicalDevice() {
    physicalDevice = PhysicalDeviceSelector::select(instance);
}

