#include "noapi_graphics_device.h"

#include <wv/graphics/Texture.h>

#include <wv/debug/log.h>
#include <wv/debug/trace.h>

#include <wv/decl.h>

#include <wv/memory/file_system.h>

#include <wv/graphics/mesh.h>
#include <wv/graphics/render_target.h>

#include <wv/device/device_context.h>
#include <wv/shader/shader_resource.h>

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

bool wv::NoAPIGraphicsDevice::initialize( LowLevelGraphicsDesc* _desc )
{
	WV_TRACE();

	Debug::Print( Debug::WV_PRINT_INFO, "Intialized Graphics Device\n" );
	Debug::Print( Debug::WV_PRINT_INFO, "  No API\n" );
	
	return true;
}

void wv::NoAPIGraphicsDevice::bindTextureToSlot( TextureID _textureID, unsigned int _slot )
{
}

void wv::NoAPIGraphicsDevice::cmdBeginRender( CmdBufferID _cmd, RenderTargetID _renderTargetID )
{
}

void wv::NoAPIGraphicsDevice::cmdEndRender( CmdBufferID _cmd )
{
}

void wv::NoAPIGraphicsDevice::cmdClearColor( CmdBufferID _cmd, float _r, float _g, float _b, float _a )
{
}

void wv::NoAPIGraphicsDevice::cmdImageClearColor( CmdBufferID _cmd, TextureID _image, float _r, float _g, float _b, float _a )
{
}

void wv::NoAPIGraphicsDevice::cmdClearDepthStencil( CmdBufferID _cmd, double _depth, uint32_t _stencil )
{
}

void wv::NoAPIGraphicsDevice::cmdImageClearDepthStencil( CmdBufferID _cmd, TextureID _image, double _depth, uint32_t _stencil )
{
}

void wv::NoAPIGraphicsDevice::cmdBindPipeline( CmdBufferID _cmd, PipelineID _pipeline )
{
}

void wv::NoAPIGraphicsDevice::cmdImageBlit( CmdBufferID _cmd, TextureID _src, TextureID _dst )
{
}

void wv::NoAPIGraphicsDevice::cmdDispatch( CmdBufferID _cmd, uint32_t _numGroupsX, uint32_t _numGroupsY, uint32_t _numGroupsZ )
{
}

void wv::NoAPIGraphicsDevice::cmdViewport( CmdBufferID _cmd, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height )
{
}

void wv::NoAPIGraphicsDevice::cmdCopyBuffer( CmdBufferID _cmd, GPUBufferID _src, GPUBufferID _dst, size_t _srcOffset, size_t _dstOffset, size_t _size )
{
}

void wv::NoAPIGraphicsDevice::cmdBindVertexBuffer( CmdBufferID _cmd, GPUBufferID _vertexBuffer )
{
}

void wv::NoAPIGraphicsDevice::cmdBindIndexBuffer( CmdBufferID _cmd, GPUBufferID _indexBuffer, size_t _offset, wv::DataType _type )
{
}

void wv::NoAPIGraphicsDevice::cmdUpdateBuffer( CmdBufferID _cmd, GPUBufferID _buffer, size_t _dataSize, void* _pData )
{
}

void wv::NoAPIGraphicsDevice::cmdUpdateSubBuffer( CmdBufferID _cmd, GPUBufferID _buffer, size_t _offset, size_t _dataSize, void* _pData )
{
}

void wv::NoAPIGraphicsDevice::cmdDraw( CmdBufferID _cmd, uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance )
{
}

void wv::NoAPIGraphicsDevice::cmdDrawIndexed( CmdBufferID _cmd, uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, int32_t _vertexOffset, uint32_t _firstInstance )
{
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::terminate()
{
	WV_TRACE();

	wv::IGraphicsDevice::terminate();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::beginRender()
{
	WV_TRACE();
	assertMainThread();

	IGraphicsDevice::beginRender();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::RenderTargetID wv::NoAPIGraphicsDevice::createRenderTarget( const RenderTargetDesc& _desc )
{
	WV_TRACE();

	return RenderTargetID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::destroyRenderTarget( RenderTargetID _renderTargetID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ShaderModuleID wv::NoAPIGraphicsDevice::createShaderModule( const ShaderModuleDesc& _desc )
{
	WV_TRACE();
	
	return ShaderModuleID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::destroyShaderModule( ShaderModuleID _programID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::PipelineID wv::NoAPIGraphicsDevice::createPipeline( const PipelineDesc& _desc )
{
	WV_TRACE();
	
	return PipelineID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::destroyPipeline( PipelineID _pipelineID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::GPUBufferID wv::NoAPIGraphicsDevice::createGPUBuffer( const GPUBufferDesc& _desc )
{
	WV_TRACE();
	
	return GPUBufferID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::destroyGPUBuffer( GPUBufferID _bufferID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bindBufferIndex( GPUBufferID _bufferID, int32_t _bindingIndex )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bufferSubData( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::TextureID wv::NoAPIGraphicsDevice::createTexture( const TextureDesc& _desc )
{
	WV_TRACE();

	return TextureID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::destroyTexture( TextureID _textureID )
{
	WV_TRACE();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::setFillMode( FillMode _mode )
{
	WV_TRACE();
}
