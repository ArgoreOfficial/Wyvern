#include "graphics_device.h"

#include <wv/debug/log.h>
#include <wv/debug/trace.h>

#include <wv/material/material.h>
#include <wv/device/device_context.h>
#include <wv/graphics/mesh.h>
#include <wv/graphics/render_target.h>

#include <wv/engine.h>
#include <wv/resource/resource_registry.h>

#include <noapi/noapi_graphics_device.h>
#if defined( WV_SUPPORT_OPENGL )
#include <opengl/opengl_graphics_device.h>
#elif defined( WV_PLATFORM_PSVITA )
#include <psp2/psp2_graphics_device.h>
#endif

#include <exception>

///////////////////////////////////////////////////////////////////////////////////////

wv::IGraphicsDevice* wv::IGraphicsDevice::createGraphics( LowLevelGraphicsDesc* _desc )
{

	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Graphics Device\n" );

	IGraphicsDevice* device = nullptr;
	switch( _desc->pContext->getGraphicsAPI() )
	{

	case GraphicsAPI::WV_GRAPHICS_API_NONE:   
		device = WV_NEW( NoAPIGraphicsDevice );
		break;

#ifdef WV_SUPPORT_OPENGL
	case GraphicsAPI::WV_GRAPHICS_API_OPENGL: 
		device = WV_NEW( GraphicsDeviceOpenGL );
		break;
#endif

#ifdef WV_PLATFORM_PSVITA
	case GraphicsAPI::WV_GRAPHICS_API_PSVITA: 
		device = WV_NEW( cPSVitaGraphicsDevice );
		break;
#endif
	}

	if( !device )
		return nullptr;

	if( !device->initialize( _desc ) )
	{
		WV_FREE( device );
		return nullptr;
	}

	device->m_threadID = std::this_thread::get_id();
	_desc->pContext->m_graphicsApiVersion = device->m_graphicsApiVersion;
	
	return device;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::initEmbeds()
{
	m_pEmptyMaterial = WV_NEW( Material, "empty", "materials/EmptyMaterial.wmat" );
	m_pEmptyMaterial->load( Engine::get()->m_pFileSystem, Engine::get()->graphics );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::executeCreateQueue()
{
	std::scoped_lock lock{ m_mutex };

	//CommandBuffer& buffer = m_commandBuffers.at( _bufferID );
	CommandBuffer& buffer = m_createDestroyCommandBuffer;
	MemoryStream& stream = buffer.getBuffer();

	for( size_t i = 0; i < buffer.numCommands(); i++ )
	{
		GPUTaskType taskType = stream.pop<GPUTaskType>();
		
		switch( taskType )
		{
		case WV_GPUTASK_NONE: 
			break;

			/// TODO: move to wv::CreateQueue
			
		case WV_GPUTASK_SET_RENDERTARGET: cmdBeginRender( 0, stream.pop<RenderTargetID>() ); break;
		case WV_GPUTASK_BIND_PIPELINE:    cmdBindPipeline( 0, stream.pop<PipelineID>() );     break;
		case WV_GPUTASK_BIND_TEXTURE: // struct { TextureID id; unsigned int slot; };
			bindTextureToSlot( stream.pop<TextureID>(), stream.pop<unsigned int>() ); 
			break;

		case WV_GPUTASK_CLEAR_RENDERTARGET: wv::Debug::Print( "error\n" ); break;
		case WV_GPUTASK_BUFFER_DATA:        wv::Debug::Print( "error\n" ); break;
		case WV_GPUTASK_DRAW:               wv::Debug::Print( "error\n" ); break;
		}
	}

	buffer.flush();
}
///////////////////////////////////////////////////////////////////////////////////////

wv::ShaderModuleID wv::IGraphicsDevice::createShaderModule( const ShaderModuleDesc& _desc )
{
	ShaderModuleID id = m_programs.emplace();
	_createShaderModule( id, _desc );
	return id;
}

void wv::IGraphicsDevice::destroyShaderModule( ShaderModuleID _programID )
{
	_destroyShaderModule( _programID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::PipelineID wv::IGraphicsDevice::createPipeline( const PipelineDesc& _desc )
{
	PipelineID id = m_pipelines.emplace();
	_createPipeline( id, _desc );
	return id;
}

void wv::IGraphicsDevice::destroyPipeline( PipelineID _pipelineID )
{
	_destroyPipeline( _pipelineID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::IGraphicsDevice::createRenderTarget( const RenderTargetDesc& _desc )
{
	RenderTargetID id = m_renderTargets.emplace();
	_createRenderTarget( id, _desc );
	return id;
}

void wv::IGraphicsDevice::destroyRenderTarget( RenderTargetID _renderTargetID )
{
	_destroyRenderTarget( _renderTargetID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GPUBufferID wv::IGraphicsDevice::createGPUBuffer( const GPUBufferDesc& _desc )
{
	GPUBufferID id = m_gpuBuffers.emplace();
	_createGPUBuffer( id, _desc );
	return id;
}

void wv::IGraphicsDevice::destroyGPUBuffer( GPUBufferID _bufferID )
{
	_destroyGPUBuffer( _bufferID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::MeshID wv::IGraphicsDevice::createMesh( const MeshDesc& _desc )
{
	MeshID id = m_meshes.emplace();
	_createMesh( id, _desc );
	return id;
}

void wv::IGraphicsDevice::destroyMesh( MeshID _meshID )
{
	_destroyMesh( _meshID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::TextureID wv::IGraphicsDevice::createTexture( const TextureDesc& _desc )
{
	TextureID id = m_textures.emplace();
	_createTexture( id, _desc );
	return id;
}

void wv::IGraphicsDevice::destroyTexture( TextureID _textureID )
{
	_destroyTexture( _textureID );
}

void wv::IGraphicsDevice::bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps )
{
	struct BufferData
	{
		TextureID tex;
		void* pData;
		bool generateMipMaps;
	} bufferData;
	bufferData.tex = _textureID;
	bufferData.pData = _pData;
	bufferData.generateMipMaps = _generateMipMaps;

	_bufferTextureData( _textureID, _pData, _generateMipMaps );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::terminate()
{
	m_pEmptyMaterial->unload( Engine::get()->m_pFileSystem, Engine::get()->graphics );
	WV_FREE( m_pEmptyMaterial );
	m_pEmptyMaterial = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::beginRender()
{
	executeCreateQueue();

	/*
	while( m_recordingCommandBuffers.size() > 0 )
	{
		wv::Time::sleepForSeconds( 0.01 );
	}

	if( m_recordingCommandBuffers.size() > 0 )
		throw std::runtime_error( "recording one or more command buffers across frames" );
	*/
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::endRender()
{
	//for( size_t i = 0; i < m_submittedCommandBuffers.size(); i++ )
	//	executeCommandBuffer( m_submittedCommandBuffers[ i ] );
	executeCreateQueue();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IGraphicsDevice::IGraphicsDevice()
{

}

wv::MeshID wv::IGraphicsDevice::_createMesh( MeshID _meshID, const MeshDesc& _desc )
{
	if( !_meshID.is_valid() )
		_meshID = m_meshes.emplace();

	Mesh mesh{};
	mesh.pMaterial   = _desc.pMaterial;
	mesh.numVertices = _desc.sizeVertices / sizeof( Vertex ); 
	
	{ // create index and vertex buffers
		GPUBufferDesc vbDesc{};
		vbDesc.name = "vertexBuffer";
		vbDesc.type = WV_BUFFER_TYPE_DYNAMIC;
		vbDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
		vbDesc.size = _desc.sizeVertices;
		mesh.vertexBufferID = createGPUBuffer( vbDesc );

		bufferSubData( mesh.vertexBufferID, _desc.vertices, _desc.sizeVertices, 0 );
	}

	if( _desc.numIndices > 0 )
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_INDICES;

		GPUBufferDesc ibDesc;
		ibDesc.name = "ebo";
		ibDesc.type = WV_BUFFER_TYPE_INDEX;
		ibDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;

		void* indices = nullptr;
		size_t sizeIndices = 0;

		if( _desc.pIndices16 )
		{
			sizeIndices = _desc.numIndices * sizeof( uint16_t );
			indices = _desc.pIndices16;
		}
		else if( _desc.pIndices32 )
		{
			sizeIndices = _desc.numIndices * sizeof( uint32_t );
			indices = _desc.pIndices32;
		}

		ibDesc.size = sizeIndices;

		mesh.indexBufferID = createGPUBuffer( ibDesc );
		mesh.numIndices = _desc.numIndices;

		bufferSubData( mesh.indexBufferID, indices, sizeIndices, 0 );
	}
	else
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_VERTICES;
	}

	if( _desc.deleteData )
	{
		if( _desc.vertices )   { WV_FREE_ARR( _desc.vertices ); }
		if( _desc.pIndices16 ) { WV_FREE_ARR( _desc.pIndices16 ); }
		if( _desc.pIndices32 ) { WV_FREE_ARR( _desc.pIndices32 ); }
	}

	// mesh.transform.update( _desc.pParentTransform );
	if( _desc.pParentTransform )
		mesh.transform.m_matrix = _desc.pParentTransform->m_matrix;

	mesh.complete = true;
	m_meshes.at( _meshID ) = mesh;

	return _meshID;
}

void wv::IGraphicsDevice::_destroyMesh( MeshID _meshID )
{
	m_meshes.erase( _meshID );
}
