#pragma once

#include <wv/Types.h>
#include <wv/Shader/Shader.h>
#include <wv/Shader/ShaderProgram.h>
#include <wv/Misc/Color.h>
#include <wv/Graphics/GPUBuffer.h>

#include <unordered_map>

#include <wv/Device/GraphicsDevice.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

#ifdef WV_SUPPORT_OPENGL

	struct sOpenGLUniformBufferData
	{
		wv::Handle blockIndex = 0;
		wv::Handle bindingIndex = 0;
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

		virtual RenderTarget* createRenderTarget( RenderTargetDesc* _desc ) override;
		virtual void destroyRenderTarget( RenderTarget** _renderTarget ) override;

		virtual void setRenderTarget( RenderTarget* _target ) override;
		virtual void setClearColor( const wv::cColor& _color ) override;
		virtual void clearRenderTarget( bool _color, bool _depth ) override;

		virtual sShaderProgram* createProgram( sShaderProgramDesc* _desc ) override;
		virtual void destroyProgram( sShaderProgram* _pProgram ) override;

		virtual sPipeline* createPipeline( sPipelineDesc* _desc ) override;
		virtual void destroyPipeline( sPipeline* _pPipeline ) override;
		virtual void bindPipeline( sPipeline* _pPipeline ) override;

		virtual cGPUBuffer* createGPUBuffer ( sGPUBufferDesc* _desc ) override;
		virtual void        allocateBuffer  ( cGPUBuffer* _buffer, size_t _size ) override;
		virtual void        bufferData      ( cGPUBuffer* _buffer ) override;
		virtual void        destroyGPUBuffer( cGPUBuffer* _buffer ) override;
		
		virtual sMesh* createMesh () override;
		virtual void   destroyMesh( sMesh** _mesh ) override;

		virtual Primitive* createPrimitive( PrimitiveDesc* _desc ) override;
		virtual void destroyPrimitive( Primitive* _primitive ) override;

		virtual void createTexture( Texture* _pTexture, TextureDesc* _desc ) override;
		virtual void destroyTexture( Texture** _texture ) override;

		virtual void bindTextureToSlot( Texture* _texture, unsigned int _slot ) override;

		virtual void drawPrimitive( Primitive* _primitive ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		virtual bool initialize( GraphicsDeviceDesc* _desc ) override;

		template<typename... Args>
		bool assertGLError( const std::string _msg, Args..._args );
		bool getError( std::string* _out );

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		/// TODO: remove?
		sPipeline* m_activePipeline = nullptr;
		RenderTarget* m_activeRenderTarget = nullptr;

		int m_numTotalUniformBlocks = 0;

		// states
		std::vector<wv::Handle> m_boundTextureSlots;
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