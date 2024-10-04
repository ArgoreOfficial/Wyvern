#include "GraphicsDevice.h"

#include <wv/Debug/Print.h>
#include <wv/Debug/Trace.h>

#include <wv/Material/Material.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Mesh/MeshResource.h>
#include <wv/Mesh/Mesh.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <wv/Engine/Engine.h>
#include <wv/Resource/ResourceRegistry.h>

#if defined( WV_SUPPORT_OPENGL )
#include <wv/Device/GraphicsDevice/OpenGLGraphicsDevice.h>
#elif defined( WV_PLATFORM_PSVITA )
#include <wv/Device/GraphicsDevice/PSVitaGraphicsDevice.h>
#endif

#include <exception>

#include <wv/Misc/Time.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::iGraphicsDevice* wv::iGraphicsDevice::createGraphicsDevice( GraphicsDeviceDesc* _desc )
{

	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Graphics Device\n" );

	iGraphicsDevice* device = nullptr;
#ifdef WV_PLATFORM_PSVITA
	device = new cPSVitaGraphicsDevice();
#else
#ifdef WV_SUPPORT_OPENGL
	device = new cOpenGLGraphicsDevice();
#endif
#endif

	if( !device )
		return nullptr;

	if( !device->initialize( _desc ) )
	{
		delete device;
		return nullptr;
	}

	device->m_threadID = std::this_thread::get_id();

	_desc->pContext->m_graphicsApiVersion = device->m_graphicsApiVersion;
	
	return device;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::initEmbeds()
{
	m_pEmptyMaterial = new cMaterial( "empty", "res/materials/EmptyMaterial.wmat" );
	m_pEmptyMaterial->load( cEngine::get()->m_pFileSystem, cEngine::get()->graphics );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::drawNode( sMeshNode* _node )
{
	WV_TRACE();

	if( !_node )
		return;

	for( auto& meshID : _node->meshes )
	{
		if( !meshID.isValid() )
			continue;

		sMesh& mesh = m_meshes.get( meshID );
		cMaterial* mat = mesh.pMaterial;

		if( mat )
		{
			if( !mat->isComplete() )
				mat = m_pEmptyMaterial;

			mat->setAsActive( this );
			mat->setInstanceUniforms( &mesh );
			draw( meshID );
		}
		else
		{
			draw( meshID );
		}
	}
	
	for( auto& childNode : _node->children )
	{
		drawNode( childNode );
	}
}


///////////////////////////////////////////////////////////////////////////////////////

wv::CmdBufferID wv::iGraphicsDevice::getCommandBuffer()
{
	m_mutex.lock();
	
	if( m_availableCommandBuffers.size() == 0 )
	{
		CmdBufferID id = m_commandBuffers.allocate( 128 );
		m_availableCommandBuffers.push( id );
	}
	
	CmdBufferID bufferIndex = m_availableCommandBuffers.front();
	m_availableCommandBuffers.pop();
	m_recordingCommandBuffers.push_back( bufferIndex );

	m_mutex.unlock();
	return bufferIndex;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::submitCommandBuffer( CmdBufferID _bufferID )
{
	m_mutex.lock();
	for( size_t i = 0; i < m_recordingCommandBuffers.size(); i++ )
	{
		if( m_recordingCommandBuffers[ i ] != _bufferID )
			continue;

		m_recordingCommandBuffers.erase( m_recordingCommandBuffers.begin() + i );
		m_submittedCommandBuffers.push_back( _bufferID );

		m_mutex.unlock();
		return;
	}
	m_mutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::executeCommandBuffer( CmdBufferID _bufferID )
{
	m_mutex.lock();
	cCommandBuffer& buffer = m_commandBuffers.get( _bufferID );
	cMemoryStream& stream = buffer.getBuffer();

	for( size_t i = 0; i < buffer.getNumCommands(); i++ )
	{
		eGPUTaskType taskType = stream.pop<eGPUTaskType>();
		
		switch( taskType )
		{
		case WV_GPUTASK_CREATE_RENDERTARGET:  
		{
			auto descData = stream.pop<sCmdCreateDesc<RenderTargetID, sRenderTargetDesc>>();
			createRenderTarget( descData.id, &descData.desc );
		} break;

		case WV_GPUTASK_CREATE_PROGRAM:
		{
			auto descData = stream.pop<sCmdCreateDesc<ProgramID, sProgramDesc>>();
			createProgram( descData.id, &descData.desc );
		} break;

		case WV_GPUTASK_CREATE_PIPELINE:
		{
			auto descData = stream.pop<sCmdCreateDesc<PipelineID, sPipelineDesc>>();
			createPipeline( descData.id, &descData.desc );
		} break;
		
		case WV_GPUTASK_CREATE_BUFFER:
		{
			auto descData = stream.pop<sCmdCreateDesc<GPUBufferID, sGPUBufferDesc>>();
			createGPUBuffer( descData.id, &descData.desc );
		} break;

		case WV_GPUTASK_CREATE_MESH:
		{
			auto descData = stream.pop<sCmdCreateDesc<MeshID, sMeshDesc>>();
			createMesh( descData.id, &descData.desc );
		} break;

		case WV_GPUTASK_CREATE_TEXTURE:
		{
			auto descData = stream.pop<sCmdCreateDesc<TextureID, sTextureDesc>>();
			createTexture( descData.id, &descData.desc );
		} break;

		case WV_GPUTASK_BUFFER_TEXTURE_DATA: // struct { TextureID tex; void* pData; bool generateMipMaps; };
		{
			TextureID& tex = *stream.pop<TextureID*>();
			void* pData = stream.pop<void*>();
			bool generateMipMaps = stream.pop<bool>();
			bufferTextureData( tex, pData, generateMipMaps );
		} break;

		case WV_GPUTASK_DESTROY_TEXTURE:      destroyTexture     ( stream.pop<TextureID>() );       break;
		case WV_GPUTASK_DESTROY_MESH:         destroyMesh        ( stream.pop<MeshID>() );          break;
		case WV_GPUTASK_DESTROY_BUFFER:       destroyGPUBuffer   ( stream.pop<GPUBufferID>() );     break;
		case WV_GPUTASK_DESTROY_PIPELINE:     destroyPipeline    ( stream.pop<PipelineID>() );      break;
		case WV_GPUTASK_DESTROY_PROGRAM:      destroyProgram     ( stream.pop<ProgramID>() ); break;
		case WV_GPUTASK_DESTROY_RENDERTARGET: destroyRenderTarget( stream.pop<RenderTargetID>() );  break;

		case WV_GPUTASK_SET_RENDERTARGET: setRenderTarget( stream.pop<RenderTargetID>() ); break;
		case WV_GPUTASK_BUFFER_DATA:      bufferData     ( stream.pop<GPUBufferID>() );    break;
		case WV_GPUTASK_BIND_PIPELINE:    bindPipeline   ( stream.pop<PipelineID>() );     break;
		case WV_GPUTASK_ALLOCATE_BUFFER: // struct { GPUBufferID id; size_t size; };
			allocateBuffer( stream.pop<GPUBufferID>(), stream.pop<size_t>() ); 
			break;
		case WV_GPUTASK_BIND_TEXTURE: // struct { TextureID id; unsigned int slot; };
			bindTextureToSlot( stream.pop<TextureID>(), stream.pop<unsigned int>() ); 
			break;
		}
	}

	for( size_t i = 0; i < m_submittedCommandBuffers.size(); i++ )
	{
		CmdBufferID submittedIndex = m_submittedCommandBuffers[ i ];
		if( submittedIndex != _bufferID )
			continue;

		m_submittedCommandBuffers.erase( m_submittedCommandBuffers.begin() + i );
		m_availableCommandBuffers.push( submittedIndex );
	}

	if( buffer.callback.m_fptr )
		buffer.callback( buffer.callbacker );

	buffer.flush();
	m_mutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ProgramID wv::iGraphicsDevice::cmdCreateProgram( CmdBufferID _bufferID, const sProgramDesc& _desc )
{
	ProgramID id = m_programs.allocate();
	bufferCmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_PROGRAM, id, _desc );
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::PipelineID wv::iGraphicsDevice::cmdCreatePipeline( CmdBufferID _bufferID, const sPipelineDesc& _desc )
{
	PipelineID id = m_pipelines.allocate();
	bufferCmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_PIPELINE, id, _desc );
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::iGraphicsDevice::cmdCreateRenderTarget( CmdBufferID _bufferID, const sRenderTargetDesc& _desc )
{
	RenderTargetID id = m_renderTargets.allocate();
	bufferCmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_RENDERTARGET, id, _desc );
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GPUBufferID wv::iGraphicsDevice::cmdCreateGPUBuffer( CmdBufferID _bufferID, const sGPUBufferDesc& _desc )
{
	GPUBufferID id = m_gpuBuffers.allocate();
	bufferCmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_BUFFER, id, _desc );
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::MeshID wv::iGraphicsDevice::cmdCreateMesh( CmdBufferID _bufferID, const sMeshDesc& _desc )
{
	MeshID  id = m_meshes.allocate();
	bufferCmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_MESH, id, _desc );
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::TextureID wv::iGraphicsDevice::cmdCreateTexture( CmdBufferID _bufferID, const sTextureDesc& _desc )
{
	TextureID id = m_textures.allocate();
	bufferCmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_TEXTURE, id, _desc );
	return id;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::setCommandBufferCallback( CmdBufferID _bufferID, wv::Function<void, void*>::fptr_t _func, void* _caller )
{
	m_mutex.lock();
	cCommandBuffer& buffer = m_commandBuffers.get( _bufferID );
	buffer.callback = _func;
	buffer.callbacker = _caller;
	m_mutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::beginRender()
{
	while( m_recordingCommandBuffers.size() > 0 )
	{
		wv::Time::sleepForSeconds( 0.01 );
	}

	if( m_recordingCommandBuffers.size() > 0 )
		throw std::runtime_error( "recording one or more command buffers across frames" );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::endRender()
{
	
	for( size_t i = 0; i < m_submittedCommandBuffers.size(); i++ )
		executeCommandBuffer( m_submittedCommandBuffers[ i ] );

}

///////////////////////////////////////////////////////////////////////////////////////

wv::iGraphicsDevice::iGraphicsDevice()
{

}

wv::MeshID wv::iGraphicsDevice::createMesh( MeshID _meshID, sMeshDesc* _desc )
{
	WV_TRACE();

	if( !_meshID.isValid() )
		_meshID = m_meshes.allocate();

	sMesh& mesh = m_meshes.get( _meshID );

	sGPUBufferDesc vbDesc;
	vbDesc.name = "vbo";
	vbDesc.type = WV_BUFFER_TYPE_VERTEX;
	vbDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;
	vbDesc.size = _desc->sizeVertices;
	mesh.vertexBufferID = createGPUBuffer( 0, &vbDesc );
	cGPUBuffer& vertexBuffer = m_gpuBuffers.get( mesh.vertexBufferID );
	mesh.pMaterial = _desc->pMaterial;

	uint32_t count = _desc->sizeVertices / sizeof( Vertex );
	vertexBuffer.count = count;

	vertexBuffer.buffer( (uint8_t*)_desc->vertices, _desc->sizeVertices );
	bufferData( mesh.vertexBufferID );
	delete[] vertexBuffer.pData;
	vertexBuffer.pData = nullptr;

	if( _desc->numIndices > 0 )
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_INDICES;

		sGPUBufferDesc ibDesc;
		ibDesc.name = "ebo";
		ibDesc.type = WV_BUFFER_TYPE_INDEX;
		ibDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;

		mesh.indexBufferID = createGPUBuffer( 0, &ibDesc );
		cGPUBuffer& indexBuffer = m_gpuBuffers.get( mesh.indexBufferID );
		indexBuffer.count = _desc->numIndices;

		if( _desc->pIndices16 )
		{
			const size_t bufferSize = _desc->numIndices * sizeof( uint16_t );

			allocateBuffer( mesh.indexBufferID, bufferSize );
			indexBuffer.buffer( _desc->pIndices16, bufferSize );
		}
		else if( _desc->pIndices32 )
		{
			const size_t bufferSize = _desc->numIndices * sizeof( uint32_t );

			allocateBuffer( mesh.indexBufferID, bufferSize );
			indexBuffer.buffer( _desc->pIndices32, bufferSize );
		}

		bufferData( mesh.indexBufferID );
		delete[] indexBuffer.pData;
		indexBuffer.pData = nullptr;
	}
	else
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_VERTICES;
	}

	if( _desc->deleteData )
	{
		if( _desc->vertices ) { delete[] _desc->vertices; }
		if( _desc->pIndices16 ) { delete[] _desc->pIndices16; }
		if( _desc->pIndices32 ) { delete[] _desc->pIndices32; }
	}

	mesh.complete = true;

	return _meshID;
}

void wv::iGraphicsDevice::destroyMesh( MeshID _meshID )
{
	WV_TRACE();

	sMesh& mesh = m_meshes.get( _meshID );

	destroyGPUBuffer( mesh.indexBufferID );
	destroyGPUBuffer( mesh.vertexBufferID );

	if( mesh.pPlatformData )
		delete mesh.pPlatformData;

	m_meshes.deallocate( _meshID );
}

void wv::iGraphicsDevice::draw( MeshID _meshID )
{
	sMesh& rMesh = m_meshes.get( _meshID );

	bindVertexBuffer( _meshID, rMesh.pMaterial->getPipeline() );

	if( rMesh.drawType == WV_MESH_DRAW_TYPE_INDICES )
	{
		cGPUBuffer& buffer = m_gpuBuffers.get( rMesh.indexBufferID );
		drawIndexed( buffer.count );
	}
	else
	{
		cGPUBuffer& buffer = m_gpuBuffers.get( rMesh.vertexBufferID );
		draw( 0, buffer.count );
	}
}
