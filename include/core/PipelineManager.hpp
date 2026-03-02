#ifndef PIPELINE_MANAGER
#define PIPELINE_MANAGER

#include <vulkan/vulkan.h>
#include <core/ShaderManager.hpp>

#include <string>
#include <utility>
#include <iostream>
<<<<<<< HEAD
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
=======
#include <glm/glm.hpp>

struct MeshPushConstants {
    glm::mat4 render_matrix;
};
>>>>>>> 735a67e ([VulkanManager] Rotating cube added and project robustness improved)



struct MeshPushConstants {
    // glm::vec4 data; // Remova ou comente este campo
    glm::mat4 render_matrix;
};
struct PipelineConfig {
  VkExtent2D extend;
  VkRenderPass renderPass;
  std::string vertexShaderPath;
  std::string fragmentShaderPath;

  VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
};


class PipelineManager {
public:
   // Cria pipeline grafico completo
   static std::pair<VkPipeline, VkPipelineLayout> createGraphicsPipeline (
      VkDevice device,
      const PipelineConfig& config
   );

   static void destroy (
      VkDevice device,
      VkPipeline pipeline,
      VkPipelineLayout layout
   );
};

#endif