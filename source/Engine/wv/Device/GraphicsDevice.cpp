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
	m_reallocatingCommandBuffers = m_availableCommandBuffers.size() == 0;

	if( m_reallocatingCommandBuffers )
	{
		cCommandBuffer buffer( m_commandBuffers.size(), 128 );
		m_commandBuffers.push_back( buffer );
		m_availableCommandBuffers.push( m_commandBuffers.size() - 1 );
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
	cCommandBuffer& buffer = m_commandBuffers[ _bufferID.value ];
	cMemoryStream& stream = buffer.getBuffer();

	for( size_t i = 0; i < buffer.getNumCommands(); i++ )
	{
		eGPUTaskType taskType = stream.pop<eGPUTaskType>();
		void** outPtr = stream.pop<void**>();

		switch( taskType )
		{
		case WV_GPUTASK_CREATE_RENDERTARGET:  
		{
			struct sDescData
			{
				RenderTargetID id;
				sRenderTargetDesc desc;
			} descData;
			descData = stream.pop<sDescData>();

			createRenderTarget( descData.id, &descData.desc );

			break;
		}

		case WV_GPUTASK_DESTROY_RENDERTARGET: destroyRenderTarget( stream.pop<RenderTargetID>() ); break;

		case WV_GPUTASK_SET_RENDERTARGET: setRenderTarget( stream.pop<RenderTargetID>() ); break;

		case WV_GPUTASK_CREATE_PROGRAM:
		{
			struct sDescData
			{
				ShaderProgramID id;
				sShaderProgramDesc desc;
			} descData;
			descData = stream.pop<sDescData>();
			createProgram( descData.id, &descData.desc );

			break;
		}

		case WV_GPUTASK_DESTROY_PROGRAM: destroyProgram( stream.pop<ShaderProgramID>() ); break;

		case WV_GPUTASK_CREATE_PIPELINE:
		{
			struct sDescData
			{
				PipelineID id;
				sPipelineDesc desc;
			} descData;
			descData = stream.pop<sDescData>();
			createPipeline( descData.id, &descData.desc );
			
			break;
		}
		case WV_GPUTASK_DESTROY_PIPELINE: 
			destroyPipeline( stream.pop<PipelineID>() );
			break;

		case WV_GPUTASK_BIND_PIPELINE: 
			bindPipeline( stream.pop<PipelineID>() );
			break;

		case WV_GPUTASK_CREATE_BUFFER:
			(GPUBufferID&)( *outPtr ) = createGPUBuffer( &stream.pop<sGPUBufferDesc>() );
			break;

		case WV_GPUTASK_ALLOCATE_BUFFER: // struct { GPUBufferID id; size_t size; };
			allocateBuffer( stream.pop<GPUBufferID>(), stream.pop<size_t>() );
			break;

		case WV_GPUTASK_BUFFER_DATA:
			bufferData( stream.pop<GPUBufferID>() );
			break;

		case WV_GPUTASK_DESTROY_BUFFER:
			destroyGPUBuffer( stream.pop<GPUBufferID>() );
			break;

		case WV_GPUTASK_CREATE_MESH:
			(MeshID&)( *outPtr ) = createMesh( &stream.pop<sMeshDesc>() );
			break;

		case WV_GPUTASK_DESTROY_MESH:
			destroyMesh( stream.pop<MeshID>() );
			break;

		case WV_GPUTASK_CREATE_TEXTURE:
			(TextureID&)( *outPtr ) = createTexture( &stream.pop<sTextureDesc>() );
			break;

		case WV_GPUTASK_BUFFER_TEXTURE_DATA: // struct { TextureID tex; void* pData; bool generateMipMaps; };
		{
			TextureID& tex = *stream.pop<TextureID*>();
			void* pData = stream.pop<void*>();
			bool generateMipMaps = stream.pop<bool>();
			bufferTextureData( tex, pData, generateMipMaps );
		} break;

		case WV_GPUTASK_DESTROY_TEXTURE:
			destroyTexture( stream.pop<TextureID>() );
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

wv::ShaderProgramID wv::iGraphicsDevice::cmdCreateProgram( CmdBufferID _bufferID, const sShaderProgramDesc& _desc )
{
	ShaderProgramID id = m_shaderPrograms.allocate();

	struct
	{
		ShaderProgramID id;
		sShaderProgramDesc desc;
	} desc;
	desc.id = id;
	desc.desc = _desc;

	bufferCommand( _bufferID, WV_GPUTASK_CREATE_PROGRAM, &desc );
	return id;
}

wv::PipelineID wv::iGraphicsDevice::cmdCreatePipeline( CmdBufferID _bufferID, const sPipelineDesc& _desc )
{
	PipelineID id = m_pipelines.allocate();

	struct
	{
		PipelineID id;
		sPipelineDesc desc;
	} desc;
	desc.id = id;
	desc.desc = _desc;

	bufferCommand( _bufferID, WV_GPUTASK_CREATE_PIPELINE, &desc );
	return id;
}

wv::RenderTargetID wv::iGraphicsDevice::cmdCreateRenderTarget( CmdBufferID _bufferID, const sRenderTargetDesc& _desc )
{
	RenderTargetID id = m_renderTargets.allocate();

	struct
	{
		RenderTargetID id;
		sRenderTargetDesc desc;
	} desc;
	desc.id = id;
	desc.desc = _desc;

	bufferCommand( _bufferID, WV_GPUTASK_CREATE_RENDERTARGET, &desc );
	return id;
}

void wv::iGraphicsDevice::setCommandBufferCallback( CmdBufferID _bufferID, wv::Function<void, void*>::fptr_t _func, void* _caller )
{
	m_mutex.lock();
	m_commandBuffers[ _bufferID.value ].callback = _func;
	m_commandBuffers[ _bufferID.value ].callbacker = _caller;
	m_mutex.unlock();
}

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

wv::iGraphicsDevice::iGraphicsDevice()
{

}
