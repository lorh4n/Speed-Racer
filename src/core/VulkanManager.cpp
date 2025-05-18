#include <core/VulkanManager.hpp>

VulkanManager::VulkanManager(int width, int height, const char* title)
    : window(width, height, title), instance(VK_NULL_HANDLE), surface(VK_NULL_HANDLE) {
}

void VulkanManager::run() {
   initVulkan();
   mainLoop();
}

void VulkanManager::mainLoop() {
    while (!window.shouldClose()) {
        window.pollEvents();
    }
}

void VulkanManager::initVulkan() {
   createInstance();
}

void VulkanManager::cleanup() {
   vkDestroyInstance(instance, nullptr);
}

VulkanManager::~VulkanManager() {
    cleanup();
}

void VulkanManager::createInstance() {
   if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error("validation layers requested, but not available!");
   }
   // Debug Callback
   auto extensions = getRequiredExtensions();
   // --------------

   VkApplicationInfo appInfo{};
   appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   appInfo.pApplicationName = "Hello Triangle";
   appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
   appInfo.pEngineName = "No Engine";
   appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
   appInfo.apiVersion = VK_API_VERSION_1_0;


   VkInstanceCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
   createInfo.pApplicationInfo = &appInfo;

   // ----- Debug Extensions ---------- //
   createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
   createInfo.ppEnabledExtensionNames = extensions.data();
    // ------------------------------- //
   uint32_t glfwExtensionCount = 0;
   const char** glfwExtensions;
   glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

   createInfo.enabledExtensionCount = glfwExtensionCount;
   createInfo.ppEnabledExtensionNames = glfwExtensions;

   createInfo.enabledLayerCount = 0;

   if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
   }
}
