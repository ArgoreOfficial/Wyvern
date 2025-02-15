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

wv::ShaderModuleID wv::NoAPIGraphicsDevice::_createShaderModule( ShaderModuleID _programID, const ShaderModuleDesc& _desc )
{
	WV_TRACE();
	
	return ShaderModuleID::InvalidID;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::NoAPIGraphicsDevice::_destroyShaderModule( ShaderModuleID _programID )
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

void wv::NoAPIGraphicsDevice::setFillMode( eFillMode _mode )
{
	WV_TRACE();
}
