#include "Graphics.h"

#include <wv/Debug/Print.h>
#include <wv/Debug/Trace.h>

#include <wv/Material/Material.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/Mesh.h>
#include <wv/Graphics/RenderTarget.h>

#include <wv/Engine/Engine.h>
#include <wv/Resource/ResourceRegistry.h>


#if defined( WV_SUPPORT_OPENGL )
#include <wv/Graphics/LowLevel/LowLevelGraphicsOpenGL.h>
#elif defined( WV_PLATFORM_PSVITA )
#include <wv/Graphics/LowLevel/PSVitaGraphicsDevice.h>
#endif

#include <exception>

#include <wv/Misc/Time.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::iLowLevelGraphics* wv::iLowLevelGraphics::createGraphics( sLowLevelGraphicsDesc* _desc )
{

	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Graphics Device\n" );

	iLowLevelGraphics* device = nullptr;
#ifdef WV_PLATFORM_PSVITA
	device = WV_NEW( cPSVitaGraphicsDevice );
#else
#ifdef WV_SUPPORT_OPENGL
	device = WV_NEW( cLowLevelGraphicsOpenGL );
#endif
#endif

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

void wv::iLowLevelGraphics::initEmbeds()
{
	m_pEmptyMaterial = WV_NEW( cMaterial, "empty", "res/materials/EmptyMaterial.wmat" );
	m_pEmptyMaterial->load( cEngine::get()->m_pFileSystem, cEngine::get()->graphics );

	sGPUBufferDesc mvbDesc{};
	mvbDesc.name  = "mvb";
	mvbDesc.type  = WV_BUFFER_TYPE_DYNAMIC;
	mvbDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
	m_vertexBuffer = _createGPUBuffer( {}, mvbDesc );

	sGPUBufferDesc mibDesc{};
	mibDesc.name  = "mib";
	mibDesc.type  = WV_BUFFER_TYPE_INDEX;
	mibDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
	m_indexBuffer = _createGPUBuffer( {}, mibDesc );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::executeCommandBuffer()
{
	std::scoped_lock lock( m_mutex );

	//cCommandBuffer& buffer = m_commandBuffers.at( _bufferID );
	cCommandBuffer& buffer = m_createDestroyCommandBuffer;
	cMemoryStream& stream = buffer.getBuffer();

	for( size_t i = 0; i < buffer.numCommands(); i++ )
	{
		eGPUTaskType taskType = stream.pop<eGPUTaskType>();
		
		switch( taskType )
		{
		case WV_GPUTASK_CREATE_RENDERTARGET:  
		{
			auto descData = stream.pop<sCmdCreateDesc<RenderTargetID, sRenderTargetDesc>>();
			_createRenderTarget( descData.id, descData.desc );
		} break;

		case WV_GPUTASK_CREATE_PROGRAM:
		{
			auto descData = stream.pop<sCmdCreateDesc<ProgramID, sProgramDesc>>();
			_createProgram( descData.id, descData.desc );
		} break;

		case WV_GPUTASK_CREATE_PIPELINE:
		{
			auto descData = stream.pop<sCmdCreateDesc<PipelineID, sPipelineDesc>>();
			_createPipeline( descData.id, descData.desc );
		} break;
		
		case WV_GPUTASK_CREATE_BUFFER:
		{
			auto descData = stream.pop<sCmdCreateDesc<GPUBufferID, sGPUBufferDesc>>();
			_createGPUBuffer( descData.id, descData.desc );
		} break;

		case WV_GPUTASK_CREATE_MESH:
		{
			auto descData = stream.pop<sCmdCreateDesc<MeshID, sMeshDesc>>();
			_createMesh( descData.id, descData.desc );
		} break;

		case WV_GPUTASK_CREATE_TEXTURE:
		{
			auto descData = stream.pop<sCmdCreateDesc<TextureID, sTextureDesc>>();
			_createTexture( descData.id, descData.desc );
		} break;

		case WV_GPUTASK_BUFFER_TEXTURE_DATA: // struct { TextureID tex; void* pData; bool generateMipMaps; };
		{
			struct BufferData
			{
				TextureID tex;
				void* pData;
				bool generateMipMaps;
			} bufferData = stream.pop<BufferData>();

			//TextureID tex = stream.pop<TextureID>();
			//void* pData = stream.pop<void*>();
			//bool generateMipMaps = stream.pop<bool>();
			_bufferTextureData( bufferData.tex, bufferData.pData, bufferData.generateMipMaps );
		} break;

		case WV_GPUTASK_DESTROY_TEXTURE:      _destroyTexture     ( stream.pop<TextureID>()      ); break;
		case WV_GPUTASK_DESTROY_MESH:         _destroyMesh        ( stream.pop<MeshID>()         ); break;
		case WV_GPUTASK_DESTROY_BUFFER:       _destroyGPUBuffer   ( stream.pop<GPUBufferID>()    ); break;
		case WV_GPUTASK_DESTROY_PIPELINE:     _destroyPipeline    ( stream.pop<PipelineID>()     ); break;
		case WV_GPUTASK_DESTROY_PROGRAM:      _destroyProgram     ( stream.pop<ProgramID>()      ); break;
		case WV_GPUTASK_DESTROY_RENDERTARGET: _destroyRenderTarget( stream.pop<RenderTargetID>() ); break;

		case WV_GPUTASK_SET_RENDERTARGET: setRenderTarget( stream.pop<RenderTargetID>() ); break;
		case WV_GPUTASK_BIND_PIPELINE:    bindPipeline   ( stream.pop<PipelineID>() );     break;
		case WV_GPUTASK_BIND_TEXTURE: // struct { TextureID id; unsigned int slot; };
			_bindTextureToSlot( stream.pop<TextureID>(), stream.pop<unsigned int>() ); 
			break;

		case WV_GPUTASK_CALLBACK:
			CreateCallback cb = stream.pop<CreateCallback>();
			cb.func( cb.caller );
			break;
		}
	}

	/*
	for( size_t i = 0; i < m_submittedCommandBuffers.size(); i++ )
	{
		CmdBufferID submittedIndex = m_submittedCommandBuffers[ i ];
		if( submittedIndex != _bufferID )
			continue;

		m_submittedCommandBuffers.erase( m_submittedCommandBuffers.begin() + i );
		m_availableCommandBuffers.push( submittedIndex );
	}
	*/

	buffer.flush();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ProgramID wv::iLowLevelGraphics::createProgram( const sProgramDesc& _desc )
{
	ProgramID id = m_programs.emplace();

	if ( std::this_thread::get_id() == getThreadID() )
		_createProgram( id, _desc );
	else
		return cmdCreateCommand( WV_GPUTASK_CREATE_PROGRAM, id, _desc );
	
	return id;
}

void wv::iLowLevelGraphics::destroyProgram( ProgramID _programID )
{
	if ( std::this_thread::get_id() == getThreadID() )
		_destroyProgram( _programID );
	else
		return cmd( WV_GPUTASK_DESTROY_PROGRAM, &_programID );

}

///////////////////////////////////////////////////////////////////////////////////////

wv::PipelineID wv::iLowLevelGraphics::createPipeline( const sPipelineDesc& _desc )
{
	PipelineID id = m_pipelines.emplace();

	if ( std::this_thread::get_id() == getThreadID() )
		_createPipeline( id, _desc );
	else
		return cmdCreateCommand( WV_GPUTASK_CREATE_PIPELINE, id, _desc );

	return id;
}

void wv::iLowLevelGraphics::destroyPipeline( PipelineID _pipelineID )
{
	if ( std::this_thread::get_id() == getThreadID() )
		_destroyPipeline( _pipelineID );
	else
		return cmd( WV_GPUTASK_DESTROY_PIPELINE, &_pipelineID );

}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::iLowLevelGraphics::createRenderTarget( const sRenderTargetDesc& _desc )
{
	RenderTargetID id = m_renderTargets.emplace();
	if ( std::this_thread::get_id() == getThreadID() )
		_createRenderTarget( id, _desc );
	else
		return cmdCreateCommand( WV_GPUTASK_CREATE_RENDERTARGET, id, _desc );
	return id;
}

void wv::iLowLevelGraphics::destroyRenderTarget( RenderTargetID _renderTargetID )
{
	if ( std::this_thread::get_id() == getThreadID() )
		_destroyRenderTarget( _renderTargetID );
	else
		return cmd( WV_GPUTASK_DESTROY_RENDERTARGET, &_renderTargetID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GPUBufferID wv::iLowLevelGraphics::createGPUBuffer( const sGPUBufferDesc& _desc )
{
	GPUBufferID id = m_gpuBuffers.emplace();
	if ( std::this_thread::get_id() == getThreadID() )
		_createGPUBuffer( id, _desc );
	else
		return cmdCreateCommand( WV_GPUTASK_CREATE_BUFFER, id, _desc );
	return id;
}

void wv::iLowLevelGraphics::destroyGPUBuffer( GPUBufferID _bufferID )
{
	if ( std::this_thread::get_id() == getThreadID() )
		_destroyGPUBuffer( _bufferID );
	else
		return cmd( WV_GPUTASK_DESTROY_BUFFER, &_bufferID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::MeshID wv::iLowLevelGraphics::createMesh( const sMeshDesc& _desc )
{
	MeshID  id = m_meshes.emplace();
	if ( std::this_thread::get_id() == getThreadID() )
		_createMesh( id, _desc );
	else
		return cmdCreateCommand( WV_GPUTASK_CREATE_MESH, id, _desc );
	return id;
}

void wv::iLowLevelGraphics::destroyMesh( MeshID _meshID )
{
	if ( std::this_thread::get_id() == getThreadID() )
		_destroyMesh( _meshID );
	else
		return cmd( WV_GPUTASK_DESTROY_MESH, &_meshID );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::TextureID wv::iLowLevelGraphics::createTexture( const sTextureDesc& _desc )
{
	TextureID id = m_textures.emplace();
	if ( std::this_thread::get_id() == getThreadID() )
		_createTexture( id, _desc );
	else
		return cmdCreateCommand( WV_GPUTASK_CREATE_TEXTURE, id, _desc );
	return id;
}

void wv::iLowLevelGraphics::destroyTexture( TextureID _textureID )
{
	if ( std::this_thread::get_id() == getThreadID() )
		_destroyTexture( _textureID );
	else
		return cmd( WV_GPUTASK_DESTROY_TEXTURE, &_textureID );
}

void wv::iLowLevelGraphics::bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps )
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

	if ( std::this_thread::get_id() == getThreadID() )
		_bufferTextureData( _textureID, _pData, _generateMipMaps );
	else
		return cmd<BufferData>( WV_GPUTASK_BUFFER_TEXTURE_DATA, &bufferData );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::cmdDrawIndexedIndirect( DrawListID _drawListID, sDrawIndexedIndirectCommand _cmd, const std::vector<sMeshInstanceData>& _instances )
{
	sDrawList& drawList = m_drawLists.at( _drawListID );
	_cmd.firstInstance = drawList.instances.size();

	for ( auto& instance : _instances )
		drawList.instances.push_back( instance );
	
	drawList.cmds.push_back( _cmd );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::queueAddCallback( wv::Function<void, void*>::fptr_t _func, void* _caller )
{
	CreateCallback cb{ _caller, _func };
	cmd( WV_GPUTASK_CALLBACK, &cb );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::beginRender()
{
	executeCommandBuffer();

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

void wv::iLowLevelGraphics::endRender()
{
	//for( size_t i = 0; i < m_submittedCommandBuffers.size(); i++ )
	//	executeCommandBuffer( m_submittedCommandBuffers[ i ] );
	executeCommandBuffer();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::iLowLevelGraphics::iLowLevelGraphics()
{

}

size_t wv::iLowLevelGraphics::pushVertexBuffer( void* _vertices, size_t _size )
{
	sGPUBuffer old = m_gpuBuffers.at( m_vertexBuffer );
	size_t base = old.size / sizeof( Vertex );

	// create new buffer
	sGPUBufferDesc desc;
	desc.name  = old.name;
	desc.type  = old.type;
	desc.usage = old.usage;
	desc.size  = old.size + _size;
	GPUBufferID mvb = _createGPUBuffer( {}, desc );

	if( old.size > 0 ) // copy old data
		copyBufferSubData( m_vertexBuffer, mvb, 0, 0, old.size );

	bufferSubData( mvb, _vertices, _size, old.size );

	// destroy and replace handle
	_destroyGPUBuffer( m_vertexBuffer );
	m_vertexBuffer = mvb;

	return base;
}

size_t wv::iLowLevelGraphics::pushIndexBuffer( void* _indices, size_t _size )
{
	sGPUBuffer old = m_gpuBuffers.at( m_indexBuffer );
	size_t base = old.size / sizeof( unsigned int );

	// create new buffer
	sGPUBufferDesc desc;
	desc.name  = old.name;
	desc.type  = old.type;
	desc.usage = old.usage;
	desc.size  = old.size + _size;
	GPUBufferID buf = _createGPUBuffer( {}, desc );

	if( old.size > 0 ) // copy old data
		copyBufferSubData( m_indexBuffer, buf, 0, 0, old.size );

	bufferSubData( buf, _indices, _size, old.size );

	// destroy and replace handle
	_destroyGPUBuffer( m_indexBuffer );
	m_indexBuffer = buf;

	return base;
}

wv::MeshID wv::iLowLevelGraphics::_createMesh( MeshID _meshID, const sMeshDesc& _desc )
{
	if( !_meshID.is_valid() )
		_meshID = m_meshes.emplace();

	sMesh mesh{};
	mesh.pMaterial   = _desc.pMaterial;
	mesh.numVertices = _desc.sizeVertices / sizeof( Vertex ); 
	mesh.baseVertex  = pushVertexBuffer( _desc.vertices, _desc.sizeVertices );
	
	if( _desc.numIndices > 0 )
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_INDICES;

		sGPUBufferDesc ibDesc;
		ibDesc.name = "ebo";
		ibDesc.type = WV_BUFFER_TYPE_INDEX;
		ibDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;

		void* indices = nullptr;
		size_t indicesSize = 0;

		if( _desc.pIndices16 )
		{
			indicesSize = _desc.numIndices * sizeof( uint16_t );
			indices = _desc.pIndices16;
		}
		else if( _desc.pIndices32 )
		{
			indicesSize = _desc.numIndices * sizeof( uint32_t );
			indices = _desc.pIndices32;
		}

		ibDesc.size = indicesSize;
		mesh.baseIndex = pushIndexBuffer( indices, indicesSize );
		mesh.numIndices = _desc.numIndices;

	}
	else
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_VERTICES;
	}

	if( _desc.deleteData )
	{
		if( _desc.vertices )   { WV_FREE( _desc.vertices ); }
		if( _desc.pIndices16 ) { WV_FREE( _desc.pIndices16 ); }
		if( _desc.pIndices32 ) { WV_FREE( _desc.pIndices32 ); }
	}

	// mesh.transform.update( _desc.pParentTransform );
	if( _desc.pParentTransform )
		mesh.transform.m_matrix = _desc.pParentTransform->m_matrix;

	mesh.complete = true;
	m_meshes.at( _meshID ) = mesh;

	return _meshID;
}

void wv::iLowLevelGraphics::_destroyMesh( MeshID _meshID )
{
	sMesh& mesh = m_meshes.at( _meshID );

	// destroy vertex and index data

	if( mesh.pPlatformData )
		WV_FREE( mesh.pPlatformData );

	m_meshes.erase( _meshID );
}
