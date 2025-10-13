#ifndef COMMAND_MANAGER
#define COMMAND_MANAGER

#include <vulkan/vulkan.h>


#include <vector>
#include <iostream>
#include <stdexcept>

#include <core/queueManager.hpp>

class CommandManager {
public:
   CommandManager(VkDevice device, QueueManager& queueManager);
   ~CommandManager();


   void createCommandPool();
   std::vector<VkCommandBuffer> allocateCommandBuffers(size_t count);

   VkCommandPool getCommandPool() const { return commandPool; }

   void cleanup();

private:
   VkDevice device;
   QueueManager& queueManager;
   VkCommandPool commandPool;
};

#endif