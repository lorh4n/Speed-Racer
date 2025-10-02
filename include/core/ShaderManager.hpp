#ifndef SHADER_MANAGER
#define SHADER_MANAGER

// #include <optional>
// #include <unordered_map>
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>
#include <string>
#include <fstream>

class ShaderManager {
   public:
   static std::vector<char> readFile(const std::string& filename);
   static VkShaderModule loadShaderModule(VkDevice device, const std::string& filepath);
   static VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

   static void destroyShaderModule(VkDevice device, VkShaderModule shaderModule);
};


#endif