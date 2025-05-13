#ifndef VULKAN_MANAGER_HPP
#define VULKAN_MANAGER_HPP

#include <iostream>

#include "WindowManager.hpp"

class VulkanManager {
   public:
      VulkanManager();
      ~VulkanManager();
      void run();
   private:
      WindowManager window;

      void initVulkan();
      void initWindow();
      void mainLoop();
      void cleanup();
};

#endif