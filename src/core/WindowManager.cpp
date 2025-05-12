#include <core/WindowManager.hpp>

WindowManager::WindowManager(int width, int height, const char* title):
   width(width), height(height), title(title) {};
   
bool WindowManager::create() {
   window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        window = nullptr;
        return false;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

    return true;
}

bool WindowManager::shouldClose() const {
    return window && glfwWindowShouldClose(window);
}

void WindowManager::swapBuffers() {
    if (window) {
        glfwSwapBuffers(window);
    }
}

void WindowManager::pollEvents() {
    glfwPollEvents();
}