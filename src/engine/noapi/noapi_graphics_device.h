#pragma once

#include <wv/types.h>
#include <wv/graphics/Pipeline.h>
#include <wv/misc/color.h>
#include <wv/graphics/gpu_buffer.h>

#include <wv/graphics/render_target.h>

#include <set>

#include <wv/graphics/graphics_device.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class NoAPIGraphicsDevice : public IGraphicsDevice
	{
	public:

		 NoAPIGraphicsDevice();
		~NoAPIGraphicsDevice() { }

		virtual void terminate() override;

		virtual void beginRender() override;

		virtual void bindBufferIndex( GPUBufferID _bufferID, int32_t _bindingIndex ) override;
		virtual void bufferSubData( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base ) override;

		virtual void bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps ) override;
		virtual void bindTextureToSlot( TextureID _textureID, unsigned int _slot )                  override;

		virtual void setFillMode( FillMode _mode ) override;

///////////////////////////////////////////////////////////////////////////////////////

		virtual RenderTargetID createRenderTarget( const RenderTargetDesc& _desc )   override;
		virtual void           destroyRenderTarget( RenderTargetID _renderTargetID ) override;

		virtual ShaderModuleID  createShaderModule( const ShaderModuleDesc& _desc ) override;
		virtual void            destroyShaderModule( ShaderModuleID _programID )     override;

		virtual PipelineID createPipeline( const PipelineDesc& _desc ) override;
		virtual void       destroyPipeline( PipelineID _pipelineID )    override;

		virtual GPUBufferID createGPUBuffer( const GPUBufferDesc& _desc ) override;
		virtual void        destroyGPUBuffer( GPUBufferID _bufferID )      override;

		virtual TextureID createTexture( const TextureDesc& _desc ) override;
		virtual void      destroyTexture( TextureID _textureID )     override;

		virtual bool initialize( LowLevelGraphicsDesc* _desc ) override;

///////////////////////////////////////////////////////////////////////////////////////

		void cmdBeginRender( CmdBufferID _cmd, RenderTargetID _renderTargetID ) override;

		void cmdEndRender( CmdBufferID _cmd ) override;

		void cmdClearColor( CmdBufferID _cmd, float _r, float _g, float _b, float _a ) override;

		void cmdImageClearColor( CmdBufferID _cmd, TextureID _image, float _r, float _g, float _b, float _a ) override;

		void cmdClearDepthStencil( CmdBufferID _cmd, double _depth, uint32_t _stencil ) override;

		void cmdImageClearDepthStencil( CmdBufferID _cmd, TextureID _image, double _depth, uint32_t _stencil ) override;

		void cmdBindPipeline( CmdBufferID _cmd, PipelineID _pipeline ) override;

		void cmdImageBlit( CmdBufferID _cmd, TextureID _src, TextureID _dst ) override;

		void cmdDispatch( CmdBufferID _cmd, uint32_t _numGroupsX, uint32_t _numGroupsY, uint32_t _numGroupsZ ) override;

		void cmdViewport( CmdBufferID _cmd, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height ) override;

		void cmdCopyBuffer( CmdBufferID _cmd, GPUBufferID _src, GPUBufferID _dst, size_t _srcOffset, size_t _dstOffset, size_t _size ) override;

		void cmdBindVertexBuffer( CmdBufferID _cmd, GPUBufferID _vertexBuffer ) override;

		void cmdBindIndexBuffer( CmdBufferID _cmd, GPUBufferID _indexBuffer, size_t _offset, wv::DataType _type ) override;

		void cmdUpdateBuffer( CmdBufferID _cmd, GPUBufferID _buffer, size_t _dataSize, void* _pData ) override;
		
		void cmdUpdateSubBuffer( CmdBufferID _cmd, GPUBufferID _buffer, size_t _offset, size_t _dataSize, void* _pData ) override;

		void cmdDraw( CmdBufferID _cmd, uint32_t _vertexCount, uint32_t _instanceCount, uint32_t _firstVertex, uint32_t _firstInstance ) override;

		void cmdDrawIndexed( CmdBufferID _cmd, uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, int32_t _vertexOffset, uint32_t _firstInstance ) override;

	};

}