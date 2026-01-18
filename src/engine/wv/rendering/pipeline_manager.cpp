#include "pipeline_manager.h"

#include <wv/rendering/renderer.h>

VkShaderModule wv::PipelineManager::createShaderModule( uint32_t* _data, size_t _dataSize )
{
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

void wv::PipelineManager::destroyPipeline( PipelineID _pipelineID )
{
	if ( !m_pipelines.contains( _pipelineID ) ) return;

	Pipeline pipeline = m_pipelines.at( _pipelineID );

	vkDestroyPipeline( m_renderer->m_device, pipeline.pipeline, nullptr );

	m_pipelines.erase( _pipelineID );
}
