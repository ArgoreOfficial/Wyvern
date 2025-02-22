#pragma once

#include <wv/types.h>

#include <wv/math/matrix.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	WV_DEFINE_ID( PipelineID );
	WV_DEFINE_ID( RenderTargetID );
	WV_DEFINE_ID( ShaderModuleID );
	WV_DEFINE_ID( GPUBufferID );
	WV_DEFINE_ID( TextureID );
	WV_DEFINE_ID( MeshID );

	WV_DEFINE_ID( BufferBindingIndex );

	struct MeshDesc;
	struct MeshNode;
	struct Mesh;

	struct TextureDesc;
	struct Texture;

	struct PipelineDesc;
	struct Pipeline;
	
	struct ShaderModuleDesc;
	struct Program;

	struct RenderTargetDesc;
	struct RenderTarget;

	struct GPUBufferDesc;

	/// TODO: move
	struct UbCameraData
	{
		Matrix4x4f projection;
		Matrix4x4f view;
		Matrix4x4f model;
	};

	enum FillMode
	{
		WV_FILL_MODE_SOLID,
		WV_FILL_MODE_WIREFRAME,
		WV_FILL_MODE_POINTS
	};

}

