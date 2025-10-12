#ifndef PIPELINE_MANAGER
#define PIPELINE_MANAGER

#include <vulkan/vulkan.h>
#include <core/ShaderManager.hpp>

#include <string>
#include <utility>
#include <iostream>

struct PipelineConfig {
  VkExtent2D extend;
  VkRenderPass renderPass;
  std::string vertexShaderPath;
  std::string fragmentShaderPath;

  VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
//   [...] lembrar de mexer aqui
};


class PipelineManager {
public:
   // Cria pipeline grafico completo
   static std::pair<VkPipeline, VkPipelineLayout> createGraphicsPipeline (
      VkDevice device,
      const PipelineConfig& config
   );

   std::pair<VkPipeline, VkPipelineLayout> createBasicGraphicsPipeline(VkDevice device, const PipelineConfig& config);

   static void destroy (
      VkDevice device,
      VkPipeline pipeline,
      VkPipelineLayout layout
   );

   VkRenderPass createRenderPass(VkDevice device, VkFormat swapchainImageFormat);
   void destroyRenderPass(VkDevice device, VkRenderPass renderPass);
};

#endif