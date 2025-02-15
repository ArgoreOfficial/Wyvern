#include "NoAPIGraphicsDevice.h"

#include <wv/Graphics/Texture.h>

#include <wv/Debug/Print.h>
#include <wv/Debug/Trace.h>

#include <wv/Decl.h>

#include <wv/Memory/FileSystem.h>

#include <wv/Graphics/Mesh.h>
#include <wv/Graphics/RenderTarget.h>

#include <wv/Device/DeviceContext.h>
#include <wv/Shader/ShaderResource.h>

#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

wv::NoAPIGraphicsDevice::NoAPIGraphicsDevice()
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::NoAPIGraphicsDevice::initialize( sLowLevelGraphicsDesc* _desc )
{
	WV_TRACE();

	Debug::Print( Debug::WV_PRINT_INFO, "Intialized Graphics Device\n" );
	Debug::Print( Debug::WV_PRINT_INFO, "  No API\n" );
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::terminate()
{
	WV_TRACE();

	wv::IGraphicsDevice::terminate();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::onResize( int _width, int _height )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::setViewport( int _width, int _height )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::beginRender()
{
	WV_TRACE();
	assertMainThread();

	IGraphicsDevice::beginRender();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::NoAPIGraphicsDevice::_createRenderTarget( RenderTargetID _renderTargetID, const sRenderTargetDesc& _desc )
{
	WV_TRACE();

	return RenderTargetID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::_destroyRenderTarget( RenderTargetID _renderTargetID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::setRenderTarget( RenderTargetID _renderTargetID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::setClearColor( const wv::cColor& _color )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::clearRenderTarget( bool _color, bool _depth )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ProgramID wv::NoAPIGraphicsDevice::_createProgram( ProgramID _programID, const sProgramDesc& _desc )
{
	WV_TRACE();
	
	return ProgramID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::_destroyProgram( ProgramID _programID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::PipelineID wv::NoAPIGraphicsDevice::_createPipeline( PipelineID _pipelineID, const sPipelineDesc& _desc )
{
	WV_TRACE();
	
	return PipelineID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::_destroyPipeline( PipelineID _pipelineID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bindPipeline( PipelineID _pipelineID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GPUBufferID wv::NoAPIGraphicsDevice::_createGPUBuffer( GPUBufferID _bufferID, const sGPUBufferDesc& _desc )
{
	WV_TRACE();
	
	return GPUBufferID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::_destroyGPUBuffer( GPUBufferID _bufferID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bindBuffer( GPUBufferID _bufferID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bindBufferIndex( GPUBufferID _bufferID, int32_t _bindingIndex )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bufferData( GPUBufferID _bufferID, void* _pData, size_t _size )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bufferSubData( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base )
{
	WV_TRACE();
}

void wv::NoAPIGraphicsDevice::copyBufferSubData( GPUBufferID _readBufferID, GPUBufferID _writeBufferID, size_t _readOffset, size_t _writeOffset, size_t _size )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::TextureID wv::NoAPIGraphicsDevice::_createTexture( TextureID _textureID, const sTextureDesc& _desc )
{
	WV_TRACE();

	return TextureID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::_bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::_destroyTexture( TextureID _textureID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::_bindTextureToSlot( TextureID _textureID, unsigned int _slot )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bindVertexBuffer( GPUBufferID _vertexPullBufferID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::setFillMode( eFillMode _mode )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::draw( uint32_t _firstVertex, uint32_t _numVertices )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::drawIndexed( uint32_t _numIndices )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::drawIndexedInstanced( uint32_t _numIndices, uint32_t _numInstances, uint32_t _baseVertex )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::multiDrawIndirect( DrawListID _drawListID )
{
	WV_TRACE();
}

