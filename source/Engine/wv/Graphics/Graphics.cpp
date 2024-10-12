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
	device = new cPSVitaGraphicsDevice();
#else
#ifdef WV_SUPPORT_OPENGL
	device = new cLowLevelGraphicsOpenGL();
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

void wv::iLowLevelGraphics::initEmbeds()
{
	m_pEmptyMaterial = new cMaterial( "empty", "res/materials/EmptyMaterial.wmat" );
	m_pEmptyMaterial->load( cEngine::get()->m_pFileSystem, cEngine::get()->graphics );

	sGPUBufferDesc mvbDesc{};
	mvbDesc.name  = "mvb";
	mvbDesc.type  = WV_BUFFER_TYPE_DYNAMIC;
	mvbDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
	m_vertexBuffer = createGPUBuffer( 0, &mvbDesc );

	sGPUBufferDesc mibDesc{};
	mibDesc.name  = "mib";
	mibDesc.type  = WV_BUFFER_TYPE_INDEX;
	mibDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
	m_indexBuffer = createGPUBuffer( 0, &mibDesc );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::drawNode( sMeshNode* _node )
{
	WV_TRACE();

	if( !_node )
		return;

	for( auto& meshID : _node->meshes )
	{
		if( !meshID.isValid() )
			continue;

		sMesh mesh = m_meshes.get( meshID );
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
		drawNode( childNode );
}


///////////////////////////////////////////////////////////////////////////////////////

wv::CmdBufferID wv::iLowLevelGraphics::getCommandBuffer()
{
	std::scoped_lock lock( m_mutex );
	
	if( m_availableCommandBuffers.size() == 0 )
	{
		CmdBufferID id = m_commandBuffers.allocate( 128 );
		m_availableCommandBuffers.push( id );
	}
	
	CmdBufferID bufferIndex = m_availableCommandBuffers.front();
	m_availableCommandBuffers.pop();
	m_recordingCommandBuffers.push_back( bufferIndex );

	return bufferIndex;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::submitCommandBuffer( CmdBufferID _bufferID )
{
	std::scoped_lock lock( m_mutex );
	for( size_t i = 0; i < m_recordingCommandBuffers.size(); i++ )
	{
		if( m_recordingCommandBuffers[ i ] != _bufferID )
			continue;

		m_recordingCommandBuffers.erase( m_recordingCommandBuffers.begin() + i );
		m_submittedCommandBuffers.push_back( _bufferID );

		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::executeCommandBuffer( CmdBufferID _bufferID )
{
	std::scoped_lock lock( m_mutex );

	cCommandBuffer& buffer = m_commandBuffers.get( _bufferID );
	cMemoryStream& stream = buffer.getBuffer();

	for( size_t i = 0; i < buffer.numCommands(); i++ )
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
		case WV_GPUTASK_BIND_PIPELINE:    bindPipeline   ( stream.pop<PipelineID>() );     break;
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
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ProgramID wv::iLowLevelGraphics::cmdCreateProgram( CmdBufferID _bufferID, const sProgramDesc& _desc )
{
	ProgramID id = m_programs.allocate();
	return cmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_PROGRAM, id, _desc );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::PipelineID wv::iLowLevelGraphics::cmdCreatePipeline( CmdBufferID _bufferID, const sPipelineDesc& _desc )
{
	PipelineID id = m_pipelines.allocate();
	return cmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_PIPELINE, id, _desc );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::iLowLevelGraphics::cmdCreateRenderTarget( CmdBufferID _bufferID, const sRenderTargetDesc& _desc )
{
	RenderTargetID id = m_renderTargets.allocate();
	return cmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_RENDERTARGET, id, _desc );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GPUBufferID wv::iLowLevelGraphics::cmdCreateGPUBuffer( CmdBufferID _bufferID, const sGPUBufferDesc& _desc )
{
	GPUBufferID id = m_gpuBuffers.allocate();
	return cmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_BUFFER, id, _desc );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::MeshID wv::iLowLevelGraphics::cmdCreateMesh( CmdBufferID _bufferID, const sMeshDesc& _desc )
{
	MeshID  id = m_meshes.allocate();
	return cmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_MESH, id, _desc );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::TextureID wv::iLowLevelGraphics::cmdCreateTexture( CmdBufferID _bufferID, const sTextureDesc& _desc )
{
	TextureID id = m_textures.allocate();
	return cmdCreateCommand( _bufferID, WV_GPUTASK_CREATE_TEXTURE, id, _desc );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::setCommandBufferCallback( CmdBufferID _bufferID, wv::Function<void, void*>::fptr_t _func, void* _caller )
{
	std::scoped_lock lock( m_mutex );
	cCommandBuffer& buffer = m_commandBuffers.get( _bufferID );
	buffer.callback = _func;
	buffer.callbacker = _caller;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::beginRender()
{
	while( m_recordingCommandBuffers.size() > 0 )
	{
		wv::Time::sleepForSeconds( 0.01 );
	}

	if( m_recordingCommandBuffers.size() > 0 )
		throw std::runtime_error( "recording one or more command buffers across frames" );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iLowLevelGraphics::endRender()
{
	for( size_t i = 0; i < m_submittedCommandBuffers.size(); i++ )
		executeCommandBuffer( m_submittedCommandBuffers[ i ] );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::iLowLevelGraphics::iLowLevelGraphics()
{

}

size_t wv::iLowLevelGraphics::pushVertexBuffer( void* _vertices, size_t _size )
{
	sGPUBuffer old = m_gpuBuffers.get( m_vertexBuffer );
	size_t base = old.size / sizeof( Vertex );

	// create new buffer
	sGPUBufferDesc desc;
	desc.name  = old.name;
	desc.type  = old.type;
	desc.usage = old.usage;
	desc.size  = old.size + _size;
	GPUBufferID mvb = createGPUBuffer( 0, &desc );

	if( old.size > 0 ) // copy old data
		copyBufferSubData( m_vertexBuffer, mvb, 0, 0, old.size );

	bufferSubData( mvb, _vertices, _size, old.size );

	// destroy and replace handle
	destroyGPUBuffer( m_vertexBuffer );
	m_vertexBuffer = mvb;

	return base;
}

size_t wv::iLowLevelGraphics::pushIndexBuffer( void* _indices, size_t _size )
{
	sGPUBuffer old = m_gpuBuffers.get( m_indexBuffer );
	size_t base = old.size / sizeof( unsigned int );

	// create new buffer
	sGPUBufferDesc desc;
	desc.name  = old.name;
	desc.type  = old.type;
	desc.usage = old.usage;
	desc.size  = old.size + _size;
	GPUBufferID buf = createGPUBuffer( 0, &desc );

	if( old.size > 0 ) // copy old data
		copyBufferSubData( m_indexBuffer, buf, 0, 0, old.size );

	bufferSubData( buf, _indices, _size, old.size );

	// destroy and replace handle
	destroyGPUBuffer( m_indexBuffer );
	m_indexBuffer = buf;

	return base;
}

wv::MeshID wv::iLowLevelGraphics::createMesh( MeshID _meshID, sMeshDesc* _desc )
{
	if( !_meshID.isValid() )
		_meshID = m_meshes.allocate();

	sMesh mesh{};
	mesh.pMaterial   = _desc->pMaterial;
	mesh.numVertices = _desc->sizeVertices / sizeof( Vertex ); 
	mesh.baseVertex  = pushVertexBuffer( _desc->vertices, _desc->sizeVertices );
	
	if( _desc->numIndices > 0 )
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_INDICES;

		sGPUBufferDesc ibDesc;
		ibDesc.name = "ebo";
		ibDesc.type = WV_BUFFER_TYPE_INDEX;
		ibDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;

		void* indices = nullptr;
		size_t indicesSize = 0;

		if( _desc->pIndices16 )
		{
			indicesSize = _desc->numIndices * sizeof( uint16_t );
			indices = _desc->pIndices16;
		}
		else if( _desc->pIndices32 )
		{
			indicesSize = _desc->numIndices * sizeof( uint32_t );
			indices = _desc->pIndices32;
		}

		ibDesc.size = indicesSize;
		mesh.baseIndex = pushIndexBuffer( indices, indicesSize );
		mesh.numIndices = _desc->numIndices;

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

	// mesh.transform.update( _desc->pParentTransform );
	if( _desc->pParentTransform )
		mesh.transform.m_matrix = _desc->pParentTransform->m_matrix;

	mesh.complete = true;
	m_meshes.get( _meshID ) = mesh;

	return _meshID;
}

void wv::iLowLevelGraphics::destroyMesh( MeshID _meshID )
{
	sMesh& mesh = m_meshes.get( _meshID );

	// destroy vertex and index data

	if( mesh.pPlatformData )
		delete mesh.pPlatformData;

	m_meshes.deallocate( _meshID );
}

void wv::iLowLevelGraphics::draw( MeshID _meshID )
{
	/*
 
	sMesh& rMesh = m_meshes.get( _meshID );

	wv::GPUBufferID SbVerticesID = rMesh.pMaterial->getShader()->getShaderBuffer( "SbVertices" );

	bindVertexBuffer( rMesh.indexBufferID, SbVerticesID );

	if( rMesh.drawType == WV_MESH_DRAW_TYPE_INDICES )
	{
		sGPUBuffer& buffer = m_gpuBuffers.get( rMesh.indexBufferID );
		drawIndexed( buffer.count );
	}
	else
	{
		sGPUBuffer& buffer = m_gpuBuffers.get( rMesh.vertexBufferID );
		draw( 0, buffer.count );
	}
	*/
}
