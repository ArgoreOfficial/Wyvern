#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Pipeline.h>
#include <wv/Misc/Color.h>
#include <wv/Graphics/GPUBuffer.h>

#include <wv/Graphics/RenderTarget.h>

#include <set>

#include <wv/Graphics/Graphics.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

#ifdef WV_SUPPORT_OPENGL

///////////////////////////////////////////////////////////////////////////////////////

	WV_DEFINE_ID( BufferBindingIndex );

///////////////////////////////////////////////////////////////////////////////////////

	struct sOpenGLBufferData
	{
		wv::Handle blockIndex = 0;
		BufferBindingIndex bindingIndex{};
	};

///////////////////////////////////////////////////////////////////////////////////////

	struct sOpenGLTextureData
	{
		unsigned int format = 0;
		unsigned int internalFormat = 0;
		unsigned int filter = 0;
		unsigned int type = 0;
	};

#endif

///////////////////////////////////////////////////////////////////////////////////////

	class cLowLevelGraphicsOpenGL : public iLowLevelGraphics
	{
	public:

		 cLowLevelGraphicsOpenGL();
		~cLowLevelGraphicsOpenGL() { }

		virtual void terminate() override;

		virtual void onResize   ( int _width, int _height ) override;
		virtual void setViewport( int _width, int _height ) override;

		virtual void beginRender() override;

		virtual RenderTargetID createRenderTarget ( RenderTargetID _renderTargetID, sRenderTargetDesc* _desc ) override;
		virtual void           destroyRenderTarget( RenderTargetID _renderTargetID )                           override;
		virtual void           setRenderTarget    ( RenderTargetID _renderTargetID )                           override;

		virtual void setClearColor    ( const wv::cColor& _color ) override;
		virtual void clearRenderTarget( bool _color, bool _depth ) override;

		virtual ProgramID createProgram ( ProgramID _programID, sProgramDesc* _desc ) override;
		virtual void      destroyProgram( ProgramID _programID )                      override;

		virtual PipelineID createPipeline ( PipelineID _pipelineID, sPipelineDesc* _desc ) override;
		virtual void       destroyPipeline( PipelineID _pipelineID )                       override;
		virtual void       bindPipeline   ( PipelineID _pipelineID )                       override;


		virtual GPUBufferID createGPUBuffer ( GPUBufferID _bufferID, sGPUBufferDesc* _desc ) override;
		virtual void        destroyGPUBuffer( GPUBufferID _bufferID )                        override;
		
		virtual void bindBuffer     ( GPUBufferID _bufferID )                        override;
		virtual void bindBufferIndex( GPUBufferID _bufferID, int32_t _bindingIndex ) override;

		virtual void bufferData   ( GPUBufferID _bufferID, void* _pData, size_t _size )               override;
		virtual void bufferSubData( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base ) override;

		virtual void copyBufferSubData( GPUBufferID _readBufferID, GPUBufferID _writeBufferID, size_t _readOffset, size_t _writeOffset, size_t _size ) override;


		virtual TextureID createTexture    ( TextureID _textureID, sTextureDesc* _pDesc )                override;
		virtual void      bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps ) override;
		virtual void      destroyTexture   ( TextureID _textureID )                                      override;
		virtual void      bindTextureToSlot( TextureID _textureID, unsigned int _slot )                  override;

		virtual void bindVertexBuffer( GPUBufferID _indexBufferID, GPUBufferID _vertexPullBufferID ) override;

		virtual void setFillMode( eFillMode _mode ) override;

		virtual void draw                ( uint32_t _firstVertex, uint32_t _numVertices ) override;
		virtual void drawIndexed         ( uint32_t _numIndices )                         override;
		virtual void drawIndexedInstanced( uint32_t _numIndices, uint32_t _numInstances, uint32_t _baseVertex ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		virtual bool initialize( sLowLevelGraphicsDesc* _desc ) override;

		template<typename... Args>
		bool assertGLError( const std::string _msg, Args..._args );
		bool getError( std::string* _out );

///////////////////////////////////////////////////////////////////////////////////////

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		wv::Handle m_vaoHandle = 0;
		cObjectHandleContainer<uint8_t, BufferBindingIndex> m_uniformBindingIndices;
		cObjectHandleContainer<uint8_t, BufferBindingIndex> m_ssboBindingIndices;
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename ...Args>
	inline bool cLowLevelGraphicsOpenGL::assertGLError( const std::string _msg, Args ..._args )
	{
		std::string error;
		if ( !getError( &error ) )
			return true;
		
		Debug::Print( Debug::WV_PRINT_ERROR, _msg.c_str(), _args... );
		Debug::Print( Debug::WV_PRINT_ERROR, "  %s\n", error.c_str() );

		return false;
	}

}