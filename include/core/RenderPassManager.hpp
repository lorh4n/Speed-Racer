#ifndef RENDERPASS_MANAGER
#define RENDERPASS_MANAGER

#include <vulkan/vulkan.h>

class RenderPassManager {
  public:
	// Cria render pass basico para swapchain
	static VkRenderPass createBasicRenderPass(
	    VkDevice device,
	    VkFormat swapchainImageFormat
   );

	// Versão Avançada com detph buffer
	static VkRenderPass createRenderPassWithDepth(
	    VkDevice device,
	    VkFormat colorFormat,
	    VkFormat depthFormat
   );


   static void destroy(VkDevice device, VkRenderPass renderPass);
};
// Conceitos importantes:
// Attachment: Descrição de imagens usadas (color, depth, stencil)
// Subpass: Operação de renderização que usa attachments
// Dependency: Como subpasses dependem uns dos outros
#endif