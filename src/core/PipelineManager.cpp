#include "core/PipelineManager.hpp"


void PipelineManager::createBasicGraphicsPipeline(VkDevice device, VkPipeline *pipeline) {
   // auto vertShaderCode = ShaderManager::readFile("../../assets/shaders/core/compiled/vert.spv");
	// auto fragShaderCode = ShaderManager::readFile("../../assets/shaders/core/compiled/frag.spv");
	// O codigo funcinou nesse formato, mas eu tenho que entender depois qual é a raiz (./) do projeto
	auto vertShaderCode = ShaderManager::readFile("../assets/shaders/core/compiled/vert.spv");
	auto fragShaderCode = ShaderManager::readFile("../assets/shaders/core/compiled/frag.spv");

	VkShaderModule vertShaderModule = ShaderManager::createShaderModule(device, vertShaderCode);
   VkShaderModule fragShaderModule = ShaderManager::createShaderModule(device, fragShaderCode);

   // --- 2. Create Shader Stage Info  --- 

   VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
   vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
   vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
   vertShaderStageInfo.module = vertShaderModule;
   vertShaderStageInfo.pName = "main"; // The entry point function in the shader


   VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
   fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
   fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
   fragShaderStageInfo.module = fragShaderModule;
   fragShaderStageInfo.pName = "main"; // The entry point function in the shader


   VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
   
   std::cout << "[PipelineManager] : Successfully created shader stages!" << std::endl;

	ShaderManager::destroyShaderModule(device, vertShaderModule);
	ShaderManager::destroyShaderModule(device, fragShaderModule);
}