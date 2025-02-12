#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Pipeline.h>
#include <wv/Misc/Color.h>
#include <wv/Graphics/GPUBuffer.h>

#include <wv/Graphics/RenderTarget.h>

#include <set>

#include <wv/Graphics/GraphicsDevice.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class NoAPIGraphicsDevice : public iLowLevelGraphics
	{
	public:

		 NoAPIGraphicsDevice();
		~NoAPIGraphicsDevice() { }

		virtual void terminate() override;

		virtual void onResize   ( int _width, int _height ) override;
		virtual void setViewport( int _width, int _height ) override;

		virtual void beginRender() override;

		virtual void setRenderTarget( RenderTargetID _renderTargetID ) override;

		virtual void setClearColor    ( const wv::cColor& _color ) override;
		virtual void clearRenderTarget( bool _color, bool _depth ) override;

		virtual void bindPipeline( PipelineID _pipelineID ) override;

		virtual void bindBuffer       ( GPUBufferID _bufferID ) override;
		virtual void bindBufferIndex  ( GPUBufferID _bufferID, int32_t _bindingIndex ) override;
		virtual void bufferData       ( GPUBufferID _bufferID, void* _pData, size_t _size ) override;
		virtual void bufferSubData    ( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base ) override;
		virtual void copyBufferSubData( GPUBufferID _readBufferID, GPUBufferID _writeBufferID, size_t _readOffset, size_t _writeOffset, size_t _size ) override;

		virtual void _bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps ) override;
		virtual void _bindTextureToSlot( TextureID _textureID, unsigned int _slot )                  override;

		virtual void bindVertexBuffer( GPUBufferID _vertexPullBufferID ) override;

		virtual void setFillMode( eFillMode _mode ) override;

		virtual void draw                ( uint32_t _firstVertex, uint32_t _numVertices ) override;
		virtual void drawIndexed         ( uint32_t _numIndices )                         override;
		virtual void drawIndexedInstanced( uint32_t _numIndices, uint32_t _numInstances, uint32_t _baseVertex ) override;

		virtual void multiDrawIndirect( DrawListID _drawListID ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:
		virtual RenderTargetID _createRenderTarget( RenderTargetID _renderTargetID, const sRenderTargetDesc& _desc ) override;
		virtual void           _destroyRenderTarget( RenderTargetID _renderTargetID )                                override;
		
		virtual ProgramID  _createProgram( ProgramID _programID, const sProgramDesc& _desc ) override;
		virtual void       _destroyProgram( ProgramID _programID )                           override;
		
		virtual PipelineID _createPipeline( PipelineID _pipelineID, const sPipelineDesc& _desc ) override;
		virtual void       _destroyPipeline( PipelineID _pipelineID )                            override;
		
		virtual GPUBufferID _createGPUBuffer( GPUBufferID _bufferID, const sGPUBufferDesc& _desc ) override;
		virtual void        _destroyGPUBuffer( GPUBufferID _bufferID )                             override;

		virtual TextureID _createTexture( TextureID _textureID, const sTextureDesc& _desc ) override;
		virtual void      _destroyTexture( TextureID _textureID )                           override;

		virtual bool initialize( sLowLevelGraphicsDesc* _desc ) override;

///////////////////////////////////////////////////////////////////////////////////////

	};

}