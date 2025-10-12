#include <core/WindowManager.hpp>

WindowManager::WindowManager(int width, int height, const std::string &title) :
    width(width), height(height), title(title), window(nullptr) {
	// Initialize GLFW
	if (!glfwInit())
	{
		throw std::runtime_error("[WindowManager] : Failed to initialize GLFW");
	}

	// Configure GLFW for Vulkan (no OpenGL context)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);        // Disable resizing for simplicity (customize as needed)

	// Create window
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("[WindowManager] : Failed to create GLFW window");
	}

	// Set user pointer for callbacks (optional, for future extensibility)
	glfwSetWindowUserPointer(window, this);
}

WindowManager::~WindowManager() {
	if (window)
	{
		glfwDestroyWindow(window);
		window = nullptr;
	}
	glfwTerminate();
}

bool WindowManager::shouldClose() const {
	return glfwWindowShouldClose(window);
}

void WindowManager::pollEvents() {
	glfwPollEvents();
}

void WindowManager::createSurface(VkInstance instance, VkSurfaceKHR *surface) const {
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("[WindowManager] : Failed to create Vulkan surface");
	}
}

std::vector<const char *> WindowManager::getRequiredExtensions() const {
	uint32_t     glfwExtensionCount = 0;
	const char **glfwExtensions     = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	if (!glfwExtensions)
	{
		throw std::runtime_error("[WindowManager] : Failed to retrieve GLFW required extensions");
	}
	return std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}