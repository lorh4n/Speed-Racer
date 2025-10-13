#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <stdexcept>

class WindowManager {
public:
    WindowManager(int width, int height, const std::string& title);
    ~WindowManager();

    // Get window dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Get the GLFW window handle
    GLFWwindow* getWindow() const { return window; }

    // Check if the window should close
    bool shouldClose() const;

    // Poll events and swap buffers
    void pollEvents();

    // Create Vulkan surface
    void createSurface(VkInstance instance, VkSurfaceKHR* surface) const;

    // Get required Vulkan instance extensions for GLFW
    std::vector<const char*> getRequiredExtensions() const;

    void setFramebufferResizeCallback(GLFWframebuffersizefun callback);

private:
    GLFWwindow* window;
    int width;
    int height;
    std::string title;
};

#endif