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
		std::string name;
		PipelineType type = WV_PIPELINE_GRAPHICS;
		PipelineTopology topology = WV_PIPELINE_TOPOLOGY_TRIANGLES;
		
		std::vector<ShaderSource> shaders{};
		std::vector<UniformBlockDesc> uniformBlocks{};
		std::vector<Uniform> textureUniforms{};
	};


	/// TODO: make not public >:(((
	class Pipeline
	{
	public:
		wv::Handle program = 0;
		uint32_t mode = 0;
		UniformBlockMap uniformBlocks;
	private:

	};
}