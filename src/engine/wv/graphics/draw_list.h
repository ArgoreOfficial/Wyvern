#pragma once

#include <wv/graphics/types.h>
#include <wv/graphics/mesh.h>

#include <vector>

namespace wv
{
	WV_DEFINE_ID( DrawListID );

	struct sDrawIndexedIndirectCommand
	{
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t  vertexOffset;
		uint32_t firstInstance;
	};

	struct sDrawIndirectCommand
	{
		uint32_t vertexCount;
		uint32_t instanceCount;
		uint32_t firstVertex;
		uint32_t firstInstance;
	};

	struct sDrawList
	{
		PipelineID pipeline;
		
		GPUBufferID viewDataBufferID;
		GPUBufferID instanceBufferID;
		GPUBufferID vertexBufferID;
		
		std::vector<sDrawIndexedIndirectCommand> cmds;
		std::vector<sMeshInstanceData> instances;
	};
}