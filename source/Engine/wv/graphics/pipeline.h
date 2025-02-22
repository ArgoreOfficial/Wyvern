#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Types.h>
#include <wv/Graphics/Program.h>

#include <wv/Graphics/VertexLayout.h>

namespace wv
{
	struct sPipeline
	{
		wv::Handle handle;
		std::string name;
		ShaderModuleID vertexProgramID{};
		ShaderModuleID fragmentProgramID{};

		void* pPlatformData;
	};

	struct sPipelineDesc
	{
		std::string name;
		sVertexLayout* pVertexLayout = nullptr;
		ShaderModuleID vertexProgramID{};
		ShaderModuleID fragmentProgramID{};

		bool reflect = true;
	};

}