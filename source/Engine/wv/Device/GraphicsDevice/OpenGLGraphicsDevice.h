#pragma once

#include <wv/Types.h>
#include <wv/Shader/Shader.h>
#include <wv/Shader/ShaderProgram.h>
#include <wv/Misc/Color.h>
#include <wv/Graphics/GPUBuffer.h>

#include <set>

#include <wv/Device/GraphicsDevice.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

#ifdef WV_SUPPORT_OPENGL

	struct sOpenGLBufferData
	{
		wv::Handle blockIndex = 0;
		wv::Handle bindingIndex = 0;
	};

	struct sOpenGLTextureData
	{
		unsigned int format = 0;
		unsigned int internalFormat = 0;
		unsigned int filter = 0;
		unsigned int type = 0;
	};

#endif

///////////////////////////////////////////////////////////////////////////////////////

	class cOpenGLGraphicsDevice : public iGraphicsDevice
	{
	public:

		cOpenGLGraphicsDevice();
		~cOpenGLGraphicsDevice() { }

		virtual void terminate() override;

		virtual void onResize( int _width, int _height ) override;
		virtual void setViewport( int _width, int _height ) override;

		virtual void beginRender() override;

		virtual RenderTarget* createRenderTarget( RenderTargetDesc* _desc ) override;
		virtual void destroyRenderTarget( RenderTarget** _renderTarget ) override;

		virtual void setRenderTarget( RenderTarget* _target ) override;
		virtual void setClearColor( const wv::cColor& _color ) override;
		virtual void clearRenderTarget( bool _color, bool _depth ) override;

		virtual ShaderProgramID createProgram( sShaderProgramDesc* _desc ) override;
		virtual void destroyProgram( ShaderProgramID _pProgram ) override;

		virtual PipelineID createPipeline ( sPipelineDesc* _desc ) override;
		virtual void       destroyPipeline( PipelineID _pipelineID ) override;
		virtual void       bindPipeline   ( PipelineID _pipelineID ) override;

		virtual cGPUBuffer* createGPUBuffer ( sGPUBufferDesc* _desc ) override;
		virtual void        allocateBuffer  ( cGPUBuffer* _buffer, size_t _size ) override;
		virtual void        bufferData      ( cGPUBuffer* _buffer ) override;
		virtual void        destroyGPUBuffer( cGPUBuffer* _buffer ) override;
		
		virtual sMesh* createMesh ( sMeshDesc* _desc )      override;
		virtual void   destroyMesh( sMesh* _pMesh ) override;

		/// TODO: texture handles?
		/// all gpu objects should be handles tbh
		 
		// Index > Ref > Pointer
		//          - graphics man 2024

		virtual sTexture createTexture    ( sTextureDesc* _pDesc )                    override;
		virtual void     bufferTextureData( sTexture* _pTexture, void* _pData, bool _generateMipMaps ) override;
		virtual void     destroyTexture   ( sTexture* _pTexture )                     override;
		virtual void     bindTextureToSlot( sTexture* _pTexture, unsigned int _slot ) override;

		virtual void bindVertexBuffer( sMesh* _pMesh, cPipelineResource* _pPipeline ) override;

		virtual void setFillMode( eFillMode _mode ) override;

		virtual void draw( sMesh* _pMesh ) override;
		virtual void drawIndexed( uint32_t _numIndices ) override;
		virtual void drawIndexedInstances( uint32_t _numIndices, uint32_t _numInstances ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		virtual bool initialize( GraphicsDeviceDesc* _desc ) override;

		template<typename... Args>
		bool assertGLError( const std::string _msg, Args..._args );
		bool getError( std::string* _out );

		wv::Handle getBufferBindingIndex();

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		/// TODO: remove?
		PipelineID m_activePipeline;
		RenderTarget* m_activeRenderTarget = nullptr;

		wv::Handle m_vaoHandle = 0;

		std::set<wv::Handle> m_usedBindingIndices;
	};
	
	template<typename ...Args>
	inline bool cOpenGLGraphicsDevice::assertGLError( const std::string _msg, Args ..._args )
	{
		std::string error;
		if ( !getError( &error ) )
			return true;
		
		Debug::Print( Debug::WV_PRINT_ERROR, _msg.c_str(), _args... );
		Debug::Print( Debug::WV_PRINT_ERROR, "  %s\n", error.c_str() );

		return false;
	}
}