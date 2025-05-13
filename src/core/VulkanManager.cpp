#include <core/VulkanManager.hpp>

VulkanManager::VulkanManager(): 
   window(800,600, "Speed Racer")
{};

void VulkanManager::run() {
   initWindow();

   mainLoop();
}

void VulkanManager::initWindow(){
   if (window.create()) {
      std::cout << "Window criada com sucesso!" << std::endl;
   }
}
void VulkanManager::mainLoop() {
   while (!window.shouldClose()) {
      glfwPollEvents();
   }
}

VulkanManager::~VulkanManager() {}