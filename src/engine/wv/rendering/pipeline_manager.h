#pragma once

#include <wv/resource_id.h>
#include <wv/helpers/unordered_array.hpp>

#include <vulkan/vulkan.h>

namespace wv {

// using PipelineID     = TResourceID<uint16_t, struct PipelineID_t>;
// using ShaderModuleID = TResourceID<uint16_t, struct ShaderModuleID_t>;

class Renderer;

struct Pipeline
{
	VkPipeline pipeline;
	VkPipelineBindPoint bindPoint;
};

class PipelineManager
{
public:
	PipelineManager( Renderer* _renderer ) : m_renderer{ _renderer } { };

	VkShaderModule createShaderModule( uint32_t* _data, size_t _dataSize );
	void destroyShaderModule( VkShaderModule _shaderModule );

	ResourceID createComputePipeline( VkShaderModule _shaderModule, VkPipelineLayout _layout, const char* _entryPoint );
	ResourceID createGraphicsPipeline( VkShaderModule _vertexShader, VkShaderModule _fragmentShader, VkPipelineLayout _layout );

	void destroyPipeline( ResourceID _pipelineID );

	Pipeline getPipeline( ResourceID _pipelineID ) const {
		if ( m_pipelines.contains( _pipelineID ) )
			return m_pipelines.at( _pipelineID );
		return {};
	}

protected:
	Renderer* m_renderer = nullptr;

	unordered_array<ResourceID, Pipeline> m_pipelines;
};

}