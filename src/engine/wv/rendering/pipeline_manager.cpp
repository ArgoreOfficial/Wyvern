#include "pipeline_manager.h"

#include <wv/rendering/renderer.h>

VkShaderModule wv::PipelineManager::createShaderModule( uint32_t* _data, size_t _dataSize )
{
	if ( _data == nullptr || _dataSize == 0 )
		return VK_NULL_HANDLE;

	VkShaderModule shaderModule{ VK_NULL_HANDLE };

	VkShaderModuleCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	createInfo.codeSize = _dataSize;
	createInfo.pCode = _data;

	if ( vkCreateShaderModule( m_renderer->m_device, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
		return VK_NULL_HANDLE;

	return shaderModule;
}

void wv::PipelineManager::destroyShaderModule( VkShaderModule _shaderModule )
{ 
	vkDestroyShaderModule( m_renderer->m_device, _shaderModule, nullptr );
}

wv::PipelineID wv::PipelineManager::createComputePipeline( VkShaderModule _shaderModule, VkPipelineLayout _layout, const char* _entryPoint )
{
	VkPipelineShaderStageCreateInfo stageInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	stageInfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
	stageInfo.module = _shaderModule;
	stageInfo.pName  = _entryPoint;

	VkComputePipelineCreateInfo compPipelinCreateInfo{ .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
	compPipelinCreateInfo.layout = _layout;
	compPipelinCreateInfo.stage = stageInfo;

	Pipeline pipeline{};
	pipeline.bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
	if ( vkCreateComputePipelines( m_renderer->m_device, VK_NULL_HANDLE, 1, &compPipelinCreateInfo, nullptr, &pipeline.pipeline ) != VK_SUCCESS )
		return {};

	return m_pipelines.emplace( pipeline );
}

wv::PipelineID wv::PipelineManager::createGraphicsPipeline( VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkPipelineLayout _layout )
{
	// Set shader stages

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	if ( _vertexShader != VK_NULL_HANDLE )
	{
		VkPipelineShaderStageCreateInfo stageInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		stageInfo.module = _vertexShader;
		stageInfo.pName = "main";
		shaderStages.push_back( stageInfo );
	}

	if ( _fragmentShader != VK_NULL_HANDLE )
	{
		VkPipelineShaderStageCreateInfo stageInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stageInfo.module = _fragmentShader;
		stageInfo.pName = "main";
		shaderStages.push_back( stageInfo );
	}

	// Set up state info

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{ .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{ .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

	VkPipelineMultisampleStateCreateInfo multisampling{ .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{ .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	VkFormat colorAttachmentformat = m_renderer->m_drawImage.imageFormat; // TODO ?

	VkPipelineRenderingCreateInfo renderInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	renderInfo.colorAttachmentCount = 1;
	renderInfo.pColorAttachmentFormats = &colorAttachmentformat;
	renderInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;

	VkPipelineDepthStencilStateCreateInfo depthStencil{ .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	depthStencil.depthTestEnable = VK_FALSE;
	depthStencil.depthWriteEnable = VK_FALSE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};
	depthStencil.back = {};
	depthStencil.minDepthBounds = 0.f;
	depthStencil.maxDepthBounds = 1.f;
	
	VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicInfo.pDynamicStates = &state[ 0 ];
	dynamicInfo.dynamicStateCount = 2;

	// Create pipeline

	VkGraphicsPipelineCreateInfo pipelineInfo{ .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineInfo.pNext = &renderInfo;

	pipelineInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineInfo.pStages    = shaderStages.data();

	pipelineInfo.pVertexInputState   = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState      = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState   = &multisampling;
	pipelineInfo.pColorBlendState    = &colorBlending;
	pipelineInfo.pDepthStencilState  = &depthStencil;
	pipelineInfo.pDynamicState       = &dynamicInfo;

	pipelineInfo.layout = _layout;

	Pipeline pipeline{};
	pipeline.bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	if ( vkCreateGraphicsPipelines( m_renderer->m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline ) != VK_SUCCESS )
	{
		// fmt::println( "failed to create pipeline" );

		return {}; // failed to create graphics pipeline
	}

	return m_pipelines.emplace( pipeline );
}

void wv::PipelineManager::destroyPipeline( PipelineID _pipelineID )
{
	if ( !m_pipelines.contains( _pipelineID ) ) return;

	Pipeline pipeline = m_pipelines.at( _pipelineID );

	vkDestroyPipeline( m_renderer->m_device, pipeline.pipeline, nullptr );

	m_pipelines.erase( _pipelineID );
}
