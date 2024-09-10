#include "GraphicsDevice.h"

#include <wv/Debug/Print.h>
#include <wv/Debug/Trace.h>

#include <wv/Assets/Materials/Material.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Primitive/Primitive.h>
#include <wv/RenderTarget/RenderTarget.h>

#if defined( WV_SUPPORT_OPENGL )
#include <wv/Device/GraphicsDevice/OpenGLGraphicsDevice.h>
#elif defined( WV_PLATFORM_PSVITA )
#include <wv/Device/GraphicsDevice/PSVitaGraphicsDevice.h>
#endif

#include <exception>

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

void wv::iGraphicsDevice::draw( sMesh* _mesh )
{
	WV_TRACE();

	if( !_mesh )
		return;

	for( size_t i = 0; i < _mesh->primitives.size(); i++ )
	{
		cMaterial* mat = _mesh->primitives[ i ]->material;
		if( mat )
		{
			if( !mat->isComplete() )
				continue;
			
			mat->setAsActive( this );
			mat->setInstanceUniforms( _mesh );
			drawPrimitive( _mesh->primitives[ i ] );
		}
		else
		{
			drawPrimitive( _mesh->primitives[ i ] );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::drawNode( sMeshNode* _node )
{
	WV_TRACE();

	if( !_node )
		return;

	for( auto& mesh : _node->meshes )
		draw( mesh );

	for( auto& childNode : _node->children )
		drawNode( childNode );
}


///////////////////////////////////////////////////////////////////////////////////////

wv::cCommandBuffer& wv::iGraphicsDevice::getCommandBuffer()
{
	if( m_availableCommandBuffers.size() == 0 )
	{
		cCommandBuffer buffer( m_commandBuffers.size(), 128 );
		m_commandBuffers.push_back( buffer );
		m_availableCommandBuffers.push( m_commandBuffers.size() - 1 );
	}

	uint32_t bufferIndex = m_availableCommandBuffers.front();
	m_availableCommandBuffers.pop();
	m_recordingCommandBuffers.push_back( bufferIndex );

	return m_commandBuffers[ bufferIndex ];
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::submitCommandBuffer( wv::cCommandBuffer& _buffer )
{
	for( size_t i = 0; i < m_recordingCommandBuffers.size(); i++ )
	{
		uint32_t index = _buffer.getIndex();

		if( m_recordingCommandBuffers[ i ] != index )
			continue;

		m_recordingCommandBuffers.erase( m_recordingCommandBuffers.begin() + i );
		m_submittedCommandBuffers.push_back( index );

		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::executeCommandBuffer( cCommandBuffer& _buffer )
{
	cMemoryStream& stream = _buffer.getBuffer();

	for( size_t i = 0; i < _buffer.getNumCommands(); i++ )
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
			destroyProgram( stream.pop<sShaderProgram*>() ); 
			break;

		case WV_GPUTASK_CREATE_PIPELINE:
			*outPtr = createPipeline( &stream.pop<sPipelineDesc>() );
			break;

		case WV_GPUTASK_DESTROY_PIPELINE: 
			destroyPipeline( stream.pop<sPipeline*>() );
			break;

		case WV_GPUTASK_BIND_PIPELINE: 
			bindPipeline( stream.pop<sPipeline*>() );
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

		case WV_GPUTASK_CREATE_PRIMITIVE:
			*outPtr = createPrimitive( &stream.pop<PrimitiveDesc>() );
			break;

		case WV_GPUTASK_DESTROY_PRIMITIVE:
			destroyPrimitive( stream.pop<Primitive**>() );
			break;

		case WV_GPUTASK_CREATE_TEXTURE:
			createTexture( (Texture*)outPtr, &stream.pop<TextureDesc>() );
			break;

		case WV_GPUTASK_DESTROY_TEXTURE:
			destroyTexture( stream.pop<Texture**>() );
			break;

		case WV_GPUTASK_BIND_TEXTURE:
			bindTextureToSlot( (Texture*)outPtr, stream.pop<unsigned int>() );
			break;
		}
	}

	for( size_t i = 0; i < m_submittedCommandBuffers.size(); i++ )
	{
		uint32_t submittedIndex = m_submittedCommandBuffers[ i ];
		if( submittedIndex != _buffer.getIndex() )
			continue;

		m_submittedCommandBuffers.erase( m_submittedCommandBuffers.begin() + i );
		m_availableCommandBuffers.push( submittedIndex );
	}

	if( _buffer.callback.m_fptr )
		_buffer.callback( _buffer.callbacker );

	_buffer.flush();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::beginRender()
{
	if( m_recordingCommandBuffers.size() > 0 )
		throw std::runtime_error( "recording one or more command buffers across frames" );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iGraphicsDevice::endRender()
{
	for( auto& submittedIndex : m_submittedCommandBuffers )
		executeCommandBuffer( m_commandBuffers[ submittedIndex ] );
}
