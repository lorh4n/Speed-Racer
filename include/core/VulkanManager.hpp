#ifndef VULKAN_MANAGER_HPP
#define VULKAN_MANAGER_HPP

#include <iostream>

class VulkanManager {
   public:
      void run();
   private:
      void initVulkan();
      void initWindow();
      void mainLoop();
      void cleanup();
};

#endif