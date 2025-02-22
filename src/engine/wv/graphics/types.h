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

	struct sMeshDesc;
	struct sMeshNode;
	struct sMesh;

	struct sTextureDesc;
	struct sTexture;

	struct sPipelineDesc;
	struct sPipeline;
	
	struct ShaderModuleDesc;
	struct sProgram;

	struct sRenderTargetDesc;
	struct sRenderTarget;

	struct sGPUBufferDesc;

	/// TODO: move
	struct sUbCameraData
	{
		Matrix4x4f projection;
		Matrix4x4f view;
		Matrix4x4f model;
	};

	enum eFillMode
	{
		WV_FILL_MODE_SOLID,
		WV_FILL_MODE_WIREFRAME,
		WV_FILL_MODE_POINTS
	};

}

