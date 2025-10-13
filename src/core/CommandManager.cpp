#include <core/CommandManager.hpp>


CommandManager::CommandManager(VkDevice device, QueueManager& queueManager) 
: device(device), queueManager(queueManager), commandPool(VK_NULL_HANDLE) {
   std::cout << "[CommandManager] : CommandManager created." << std::endl;
}

CommandManager::~CommandManager() {
   cleanup();
}


void CommandManager::createCommandPool() {
   auto queueFamilies = queueManager.getQueueFamilies();
   uint32_t graphicsFamily = queueFamilies.at(QueueType::GRAPHICS).index;

   VkCommandPoolCreateInfo poolInfo{};
   poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
   poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
   poolInfo.queueFamilyIndex = graphicsFamily;

   if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
      throw std::runtime_error("[CommandManager] : Failed to create command pool!");
   } 
   std::cout << "[CommandManager] : Command pool created." << std::endl;
}

std::vector<VkCommandBuffer> CommandManager::allocateCommandBuffers(size_t count) {
   std::vector<VkCommandBuffer> commandBuffers(count);

   VkCommandBufferAllocateInfo allocInfo{};
   allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
   allocInfo.commandPool = commandPool;
   allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
   allocInfo.commandBufferCount = static_cast<uint32_t>(count);

   if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("[CommandManager] : Failed to allocate command buffers!");
   }

   std::cout << "[CommandManager] : Command buffers allocated (" << count << " buffers)." << std::endl;

   return commandBuffers;
}

void CommandManager::cleanup() {
   if (commandPool != VK_NULL_HANDLE) {
      vkDestroyCommandPool(device, commandPool, nullptr);
      std::cout << "[CommandManager] : Command pool destroyed." << std::endl;
      commandPool = VK_NULL_HANDLE;
   }
   
}