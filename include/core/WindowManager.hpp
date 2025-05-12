#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP


#define GLFW_INCLUDE_VULKAN
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class WindowManager {
   public:
      WindowManager(int width, int height, const char* title);
      ~WindowManager();

      bool create();

      int getWidth() const { return  width;};
      int getHeight() const { return  height;};

      GLFWwindow* getWindow() const { return window;};
      bool shouldClose() const;

      // Troca os buffers e processa eventos
      void swapBuffers();
      void pollEvents();
   private:
      GLFWwindow* window;
      int width;
      int height;
      const char* title;
};

#endif