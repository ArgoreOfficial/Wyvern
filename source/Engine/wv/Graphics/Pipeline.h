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
		ProgramID vertexProgramID{};
		ProgramID fragmentProgramID{};

		void* pPlatformData;
	};

	struct sPipelineDesc
	{
		std::string name;
		sVertexLayout* pVertexLayout = nullptr;
		ProgramID vertexProgramID{};
		ProgramID fragmentProgramID{};

		bool reflect = true;
	};

}