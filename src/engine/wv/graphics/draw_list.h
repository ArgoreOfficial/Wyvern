#pragma once

#include <wv/graphics/types.h>
#include <wv/graphics/mesh.h>

#include <vector>

namespace wv
{
	WV_DEFINE_ID( DrawListID );

	struct DrawIndexedIndirectCommand
	{
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t  vertexOffset;
		uint32_t firstInstance;
	};

	struct DrawIndirectCommand
	{
		uint32_t vertexCount;
		uint32_t instanceCount;
		uint32_t firstVertex;
		uint32_t firstInstance;
	};

	struct DrawList
	{
		PipelineID pipeline;
		
		GPUBufferID viewDataBufferID;
		GPUBufferID instanceBufferID;
		GPUBufferID vertexBufferID;
		
		std::vector<DrawIndexedIndirectCommand> cmds;
		std::vector<MeshInstanceData> instances;
	};
}