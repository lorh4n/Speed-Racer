#include "core/PipelineManager.hpp"


std::pair<VkPipeline, VkPipelineLayout> PipelineManager::createBasicGraphicsPipeline(VkDevice device, const PipelineConfig& config) {
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


   // ============================= Fixed Functions =====================================
   // https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions

   // ------------------------------ Dynamic State --------------------------------------

   std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
   };

   VkPipelineDynamicStateCreateInfo dynamicState{};
   dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()); // Uso de static_cast, versão mais segura de casting ao inves de um simples (uint32_t) variavel
   dynamicState.pDynamicStates = dynamicStates.data();

   // ------------------------------ Vertex Input ---------------------------------------

   VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
   vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vertexInputInfo.vertexBindingDescriptionCount = 0;
   vertexInputInfo.pVertexAttributeDescriptions = nullptr;
   vertexInputInfo.vertexAttributeDescriptionCount = 0;
   vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

   // ------------------------------ Input Assembly -------------------------------------

   VkPipelineInputAssemblyStateCreateInfo inputAssembly{}; 
   inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
   inputAssembly.primitiveRestartEnable = VK_FALSE;
   
   // ------------------------------ Viewports and Scissors -----------------------------

   VkViewport viewport{};
   viewport.x = 0.0f;
   viewport.y = 0.0f;
   viewport.width = static_cast<float>(config.extend.width);
   viewport.height = static_cast<float>(config.extend.height);
   viewport.minDepth = 0.0f;
   viewport.maxDepth = 1.0f;

   VkRect2D scissor{};
   scissor.offset = {0, 0};
   scissor.extent = config.extend;

   VkPipelineViewportStateCreateInfo viewportState{};
   viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   viewportState.viewportCount = 1;
   viewportState.pViewports = &viewport;
   viewportState.scissorCount = 1;
   viewportState.pScissors = &scissor;

   // ------------------------------ Rasterizer -----------------------------------------
   VkPipelineRasterizationStateCreateInfo rasterizer{};
   rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   rasterizer.depthClampEnable = VK_FALSE;
   rasterizer.rasterizerDiscardEnable = VK_FALSE;
   rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
   rasterizer.lineWidth = 1.0f;
   rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
   rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

   rasterizer.depthBiasEnable = VK_FALSE;
   rasterizer.depthBiasConstantFactor = 0.0f; // Optional
   rasterizer.depthBiasClamp = 0.0f; // Optional
   rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


   // ------------------------------ Multisampling --------------------------------------
   VkPipelineMultisampleStateCreateInfo multisampling{};
   multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   multisampling.sampleShadingEnable = VK_FALSE;
   multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
   multisampling.minSampleShading = 1.0f; // Optional
   multisampling.pSampleMask = nullptr; // Optional
   multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
   multisampling.alphaToOneEnable = VK_FALSE; // Optional

   // ------------------------------ Depth and stencil testing --------------------------
   // Implementar depois

   // ------------------------------ Color blending --------------------------------------
   VkPipelineColorBlendAttachmentState colorBlendAttachment{};
   colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
                                       VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
   colorBlendAttachment.blendEnable = VK_FALSE;

   VkPipelineColorBlendStateCreateInfo colorBlending{};
   colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   colorBlending.logicOpEnable = VK_FALSE;
   colorBlending.logicOp = VK_LOGIC_OP_COPY;
   colorBlending.attachmentCount = 1;
   colorBlending.pAttachments = &colorBlendAttachment;
   colorBlending.blendConstants[0] = 0.0f;
   colorBlending.blendConstants[1] = 0.0f;
   colorBlending.blendConstants[2] = 0.0f;
   colorBlending.blendConstants[3] = 0.0f;

   // ------------------------------ Pipeline Layout --------------------------------------

   VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
   pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   pipelineLayoutInfo.setLayoutCount = 0;
   pipelineLayoutInfo.pSetLayouts = nullptr;
   pipelineLayoutInfo.pushConstantRangeCount = 0;
   pipelineLayoutInfo.pPushConstantRanges = nullptr;

   VkPipelineLayout pipelineLayout;
   if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("[PipelineManager] Failed to create pipeline layout!");
   }

     // ========== 9. GRAPHICS PIPELINE CREATION ==========
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = config.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VkPipeline graphicsPipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        throw std::runtime_error("[PipelineManager] Failed to create graphics pipeline!");
    }


	
   
   
   
   ShaderManager::destroyShaderModule(device, vertShaderModule);
	ShaderManager::destroyShaderModule(device, fragShaderModule);

   return std::make_pair(graphicsPipeline, pipelineLayout);
}

VkRenderPass PipelineManager::createRenderPass(VkDevice device, VkFormat swapchainImageFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

    return renderPass;
}

void PipelineManager::destroyRenderPass(VkDevice device, VkRenderPass renderPass) {
    vkDestroyRenderPass(device, renderPass, nullptr);
}
