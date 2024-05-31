#pragma once

#include <wv/Shader/UniformBlock.h>

namespace wv
{
	struct ShaderSource;

	typedef void( *PipelineUniformCallback )( wv::UniformBlockMap& _uniformBlocks );

	enum PipelineType
	{
		WV_PIPELINE_GRAPHICS
	};

	enum PipelineTopology
	{
		WV_PIPELINE_TOPOLOGY_TRIANGLES
	};

	struct PipelineDesc
	{
		PipelineType type = WV_PIPELINE_GRAPHICS;
		PipelineTopology topology = WV_PIPELINE_TOPOLOGY_TRIANGLES;
		
		ShaderSource* shaders = nullptr;
		unsigned int numShaders = 0;

		UniformBlockDesc* uniformBlocks = nullptr;
		unsigned int numUniformBlocks = 0;

		Uniform* textureUniforms = nullptr;
		unsigned int numTextureUniforms = 0;

		PipelineUniformCallback pipelineCallback = nullptr;
		PipelineUniformCallback instanceCallback = nullptr;
	};


	/// TODO: make not public >:(((
	class Pipeline
	{
	public:
		wv::Handle program = 0;
		uint32_t mode = 0;
		UniformBlockMap uniformBlocks;

		PipelineUniformCallback pipelineCallback = nullptr;
		PipelineUniformCallback instanceCallback = nullptr;
	private:

	};
}