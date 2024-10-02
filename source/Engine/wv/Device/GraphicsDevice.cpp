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

	for( auto& mesh : _node->meshes )
	{
		if( mesh == nullptr )
			continue;

		cMaterial* mat = mesh->pMaterial;
		if( mat )
		{
			if( !mat->isComplete() )
				mat = m_pEmptyMaterial;

			mat->setAsActive( this );
			mat->setInstanceUniforms( mesh );
			draw( mesh );
		}
		else
		{
			draw( mesh );
		}
	}
	
	for( auto& childNode : _node->children )
	{
		drawNode( childNode );
	}
}


///////////////////////////////////////////////////////////////////////////////////////

uint32_t wv::iGraphicsDevice::getCommandBuffer()
{
	m_mutex.lock();
	m_reallocatingCommandBuffers = m_availableCommandBuffers.size() == 0;

	if( m_reallocatingCommandBuffers )
	{
		cCommandBuffer buffer( m_commandBuffers.size(), 128 );
		m_commandBuffers.push_back( buffer );
		m_availableCommandBuffers.push( m_commandBuffers.size() - 1 );
	}

	uint32_t bufferIndex = m_availableCommandBuffers.front();
	m_availableCommandBuffers.pop();
	m_recordingCommandBuffers.push_back( bufferIndex );

	m_mutex.unlock();
	return bufferIndex;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::submitCommandBuffer( uint32_t& _buffer )
{
	m_mutex.lock();
	for( size_t i = 0; i < m_recordingCommandBuffers.size(); i++ )
	{
		if( m_recordingCommandBuffers[ i ] != _buffer )
			continue;

		m_recordingCommandBuffers.erase( m_recordingCommandBuffers.begin() + i );
		m_submittedCommandBuffers.push_back( _buffer );

		m_mutex.unlock();
		return;
	}
	m_mutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::executeCommandBuffer( uint32_t _index )
{
	m_mutex.lock();
	cCommandBuffer& buffer = m_commandBuffers[ _index ];
	cMemoryStream& stream = buffer.getBuffer();

	for( size_t i = 0; i < buffer.getNumCommands(); i++ )
	{
		eGPUTaskType taskType = stream.pop<eGPUTaskType>();
		void** outPtr = stream.pop<void**>();

		switch( taskType )
		{
		case WV_GPUTASK_CREATE_RENDERTARGET:  
			*outPtr = createRenderTarget( &stream.pop<RenderTargetDesc>() ); 
			break;

		case WV_GPUTASK_DESTROY_RENDERTARGET: 
			destroyRenderTarget( stream.pop<RenderTarget**>() );
			break;

		case WV_GPUTASK_SET_RENDERTARGET: 
			setRenderTarget( stream.pop<RenderTarget*>() ); 
			break;

		//case WV_GPUTASK_CLEAR_RENDERTARGET: break

		case WV_GPUTASK_CREATE_PROGRAM: 
			*outPtr = createProgram( &stream.pop<sShaderProgramDesc>() );
			break;

		case WV_GPUTASK_DESTROY_PROGRAM: 
			destroyProgram( stream.pop<ShaderProgramID>() ); 
			break;

		case WV_GPUTASK_CREATE_PIPELINE:
			*outPtr = createPipeline( &stream.pop<sPipelineDesc>() );
			break;

		case WV_GPUTASK_DESTROY_PIPELINE: 
			destroyPipeline( stream.pop<PipelineID>() );
			break;

		case WV_GPUTASK_BIND_PIPELINE: 
			bindPipeline( stream.pop<PipelineID>() );
			break;

		case WV_GPUTASK_CREATE_BUFFER:
			*outPtr = createGPUBuffer( &stream.pop<sGPUBufferDesc>() );
			break;

		case WV_GPUTASK_ALLOCATE_BUFFER:
			allocateBuffer( (cGPUBuffer*)outPtr, stream.pop<size_t>() );
			break;

		case WV_GPUTASK_BUFFER_DATA:
			bufferData( stream.pop<cGPUBuffer*>() );
			break;

		case WV_GPUTASK_DESTROY_BUFFER:
			destroyGPUBuffer( stream.pop<cGPUBuffer*>() );
			break;

		case WV_GPUTASK_CREATE_MESH:
			*outPtr = createMesh( &stream.pop<sMeshDesc>() );
			break;

		case WV_GPUTASK_DESTROY_MESH:
			destroyMesh( stream.pop<sMesh*>() );
			break;

		case WV_GPUTASK_CREATE_TEXTURE:
			(sTexture&)*outPtr = createTexture( &stream.pop<sTextureDesc>() );
			break;

		case WV_GPUTASK_BUFFER_TEXTURE_DATA:
		{
			sTexture* tex = stream.pop<sTexture*>();
			void* pData = stream.pop<void*>();
			bool generateMipMaps = stream.pop<bool>();
			bufferTextureData( tex, pData, generateMipMaps );
		} break;

		case WV_GPUTASK_DESTROY_TEXTURE:
			destroyTexture( stream.pop<sTexture*>() );
			break;

		case WV_GPUTASK_BIND_TEXTURE:
			bindTextureToSlot( (sTexture*)outPtr, stream.pop<unsigned int>() );
			break;
		}
	}

	for( size_t i = 0; i < m_submittedCommandBuffers.size(); i++ )
	{
		uint32_t submittedIndex = m_submittedCommandBuffers[ i ];
		if( submittedIndex != _index )
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

void wv::iGraphicsDevice::setCommandBufferCallback( uint32_t& _buffer, wv::Function<void, void*>::fptr_t _func, void* _caller )
{
	m_mutex.lock();
	m_commandBuffers[ _buffer ].callback = _func;
	m_commandBuffers[ _buffer ].callbacker = _caller;
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
