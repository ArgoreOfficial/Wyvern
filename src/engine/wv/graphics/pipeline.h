#pragma once

#include <wv/types.h>
#include <wv/graphics/types.h>
#include <wv/graphics/Program.h>

#include <wv/graphics/vertex_layout.h>

namespace wv
{
	struct Pipeline
	{
		wv::Handle handle;
		std::string name;
		ShaderModuleID vertexProgramID{};
		ShaderModuleID fragmentProgramID{};

		void* pPlatformData;
	};

	struct PipelineDesc
	{
		std::string name;
		VertexLayout* pVertexLayout = nullptr;
		ShaderModuleID vertexProgramID{};
		ShaderModuleID fragmentProgramID{};

		bool reflect = true;
	};

}