#include <core/WindowManager.hpp>

WindowManager::WindowManager(int width, int height, const char* title)
    : width(width), height(height), title(title), window(nullptr) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Usamos Vulkan
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // Janela fixa por enquanto

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
}

WindowManager::~WindowManager() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool WindowManager::shouldClose() const {
    return window && glfwWindowShouldClose(window);
}

void WindowManager::pollEvents() {
    glfwPollEvents();
}