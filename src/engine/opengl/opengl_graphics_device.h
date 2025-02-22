#pragma once

#include <wv/types.h>
#include <wv/graphics/Pipeline.h>
#include <wv/misc/color.h>
#include <wv/graphics/gpu_buffer.h>

#include <wv/graphics/render_target.h>
#include <wv/graphics/graphics_device.h>

#include <set>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

#ifdef WV_SUPPORT_OPENGL

///////////////////////////////////////////////////////////////////////////////////////

	struct PlatformTextureData
	{
		unsigned int format = 0;
		unsigned int internalFormat = 0;
		unsigned int filter = 0;
		unsigned int type = 0;
	};

#endif

///////////////////////////////////////////////////////////////////////////////////////

	class GraphicsDeviceOpenGL : public IGraphicsDevice
	{
	public:

		 GraphicsDeviceOpenGL();
		~GraphicsDeviceOpenGL() { }

		virtual void terminate() override;

		virtual void beginRender() override;

		virtual void bindBufferIndex  ( GPUBufferID _bufferID, int32_t _bindingIndex ) override;
		virtual void bufferSubData    ( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base ) override;
		
		virtual void _bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps ) override;
		virtual void bindTextureToSlot( TextureID _textureID, unsigned int _slot )                  override;

		virtual void setFillMode( FillMode _mode ) override;

///////////////////////////////////////////////////////////////////////////////////////

		virtual RenderTargetID _createRenderTarget( RenderTargetID _renderTargetID, const RenderTargetDesc& _desc ) override;
		virtual void           _destroyRenderTarget( RenderTargetID _renderTargetID )                                override;
		
		virtual ShaderModuleID  _createShaderModule( ShaderModuleID _programID, const ShaderModuleDesc& _desc ) override;
		virtual void       _destroyShaderModule( ShaderModuleID _programID )                           override;
		
		virtual PipelineID _createPipeline( PipelineID _pipelineID, const PipelineDesc& _desc ) override;
		virtual void       _destroyPipeline( PipelineID _pipelineID )                            override;
		
		virtual GPUBufferID _createGPUBuffer( GPUBufferID _bufferID, const GPUBufferDesc& _desc ) override;
		virtual void        _destroyGPUBuffer( GPUBufferID _bufferID )                             override;

		virtual TextureID _createTexture( TextureID _textureID, const TextureDesc& _desc ) override;
		virtual void      _destroyTexture( TextureID _textureID )                           override;

		virtual bool initialize( LowLevelGraphicsDesc* _desc ) override;

		template<typename... Args>
		bool assertGLError( const std::string _msg, Args..._args );
		bool getError( std::string* _out );

///////////////////////////////////////////////////////////////////////////////////////

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		wv::Handle m_vaoHandle = 0;
		wv::unordered_array<BufferBindingIndex, uint8_t> m_uniformBindingIndices;
		wv::unordered_array<BufferBindingIndex, uint8_t> m_ssboBindingIndices;

		wv::DataType m_indexBufferDataType;

		// Inherited via IGraphicsDevice
		void cmdBeginRender( 
			CmdBufferID _cmd, 
			RenderTargetID _renderTargetID ) override;

		void cmdEndRender( 
			CmdBufferID _cmd ) override;
		
		void cmdClearColor( 
			CmdBufferID _cmd, 
			float _r, 
			float _g, 
			float _b, 
			float _a ) override;

		void cmdImageClearColor( 
			CmdBufferID _cmd, 
			TextureID _image, 
			float _r, 
			float _g, 
			float _b, 
			float _a ) override;

		void cmdClearDepthStencil( 
			CmdBufferID _cmd, 
			double _depth, 
			uint32_t _stencil ) override;

		void cmdImageClearDepthStencil( 
			CmdBufferID _cmd, 
			TextureID _image, 
			double _depth, 
			uint32_t _stencil ) override;
		
		void cmdBindPipeline( 
			CmdBufferID _cmd, 
			PipelineID _pipeline ) override;

		void cmdImageBlit( 
			CmdBufferID _cmd, 
			TextureID _src, 
			TextureID _dst ) override;
		
		void cmdDispatch( 
			CmdBufferID _cmd, 
			uint32_t _numGroupsX, 
			uint32_t _numGroupsY, 
			uint32_t _numGroupsZ ) override;

		void cmdViewport( 
			CmdBufferID _cmd, 
			uint32_t _x, 
			uint32_t _y, 
			uint32_t _width, 
			uint32_t _height ) override;

		void cmdCopyBuffer( 
			CmdBufferID _cmd, 
			GPUBufferID _src, 
			GPUBufferID _dst, 
			size_t _srcOffset, 
			size_t _dstOffset, 
			size_t _size ) override;

		void cmdBindVertexBuffer( 
			CmdBufferID _cmd, 
			GPUBufferID _vertexBuffer ) override;

		void cmdBindIndexBuffer( 
			CmdBufferID _cmd, 
			GPUBufferID _indexBuffer, 
			size_t _offset, 
			wv::DataType _type ) override;

		void cmdUpdateBuffer( CmdBufferID _cmd, GPUBufferID _buffer, size_t _size, void* _pData ) override;
		void cmdUpdateSubBuffer( CmdBufferID _cmd, GPUBufferID _buffer, size_t _offset, size_t _size, void* _pData ) override;
		
		void cmdDraw( 
			CmdBufferID _cmd, 
			uint32_t _vertexCount, 
			uint32_t _instanceCount, 
			uint32_t _firstVertex, 
			uint32_t _firstInstance ) override;

		void cmdDrawIndexed( 
			CmdBufferID _cmd, 
			uint32_t _indexCount, 
			uint32_t _instanceCount, 
			uint32_t _firstIndex, 
			int32_t _vertexOffset, 
			uint32_t _firstInstance ) override;
};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename ...Args>
	inline bool GraphicsDeviceOpenGL::assertGLError( const std::string _msg, Args ..._args )
	{
		std::string error;
		if ( !getError( &error ) )
			return true;
		
		Debug::Print( Debug::WV_PRINT_ERROR, _msg.c_str(), _args... );
		Debug::Print( Debug::WV_PRINT_ERROR, "  %s\n", error.c_str() );

		return false;
	}

}