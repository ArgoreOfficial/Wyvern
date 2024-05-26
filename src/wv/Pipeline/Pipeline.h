#pragma once

namespace wv
{
	struct ShaderSource;

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
		PipelineType type;
		PipelineTopology topology;

		ShaderSource* shaders;
		int numShaders;

		int layout;
	};

	class Pipeline
	{
	public:
		wv::Handle program;
		uint32_t mode;
	private:

	};
}