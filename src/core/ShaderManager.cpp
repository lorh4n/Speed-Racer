#include <core/ShaderManager.hpp>


std::vector<char> ShaderManager::readFile(const std::string& filename) {
   std::ifstream file(filename, std::ios::ate | std::ios::binary);

   if(!file.is_open()) {
      throw std::runtime_error("[ShaderManager]: failed to open File");
   }

   size_t fileSize = (size_t) file.tellg();
   std::vector<char> buffer(fileSize);

   // Volta para o início do arquivo para poder ler do começo
   file.seekg(0);
   // Lê 'fileSize' bytes do arquivo e os coloca diretamente no 'buffer'
   file.read(buffer.data(), fileSize);

   file.close();
   return buffer;
}

VkShaderModule ShaderManager::createShaderModule(VkDevice device, const std::vector<char>& code) {
   VkShaderModuleCreateInfo createInfo{};
   createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   createInfo.codeSize = code.size();
   createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); 

   VkShaderModule shaderModule;
   if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
      throw std::runtime_error("[ShaderManager] failed to create Shader Module");
   }

   return shaderModule;   
}
