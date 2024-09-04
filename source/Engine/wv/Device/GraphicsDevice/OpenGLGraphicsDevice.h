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

		virtual bool createShader( cShader* _shader, eShaderType _type ) override;
		virtual void destroyShader( cShader* _shader ) override;
		virtual void compileShader( cShader* _shader ) override;

		virtual void createProgram( wv::cShaderProgram* _program, const std::string& _name ) override;
		virtual void destroyProgram( cShaderProgram* _program ) override;
		virtual void linkProgram( cShaderProgram* _program, std::vector<UniformBlockDesc> _uniformBlocks = {}, std::vector<Uniform> _textureUniforms = { } ) override;
		virtual void useProgram( cShaderProgram* _program ) override;

		virtual sGPUBuffer* createGPUBuffer( eGPUBufferType _type ) override;
		virtual void bufferData( sGPUBuffer* _buffer, void* _data, size_t _size ) override;
		virtual void destroyGPUBuffer( sGPUBuffer* _buffer ) override;
		
		virtual Mesh* createMesh( MeshDesc* _desc ) override;
		virtual void destroyMesh( Mesh** _mesh ) override;

		virtual Primitive* createPrimitive( PrimitiveDesc* _desc, Mesh* _mesh ) override;
		virtual void destroyPrimitive( Primitive** _primitive ) override;


		virtual void createTexture( Texture* _pTexture, TextureDesc* _desc ) override;
		virtual void destroyTexture( Texture** _texture ) override;

		virtual void bindTextureToSlot( Texture* _texture, unsigned int _slot ) override;

		virtual void draw( Mesh* _mesh ) override;

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		virtual bool initialize( GraphicsDeviceDesc* _desc ) override;

		template<typename... Args>
		bool assertGLError( const std::string _msg, Args..._args );
		bool getError( std::string* _out );

		void drawPrimitive( Primitive* _primitive );
		UniformBlock createUniformBlock( cShaderProgram* _program, UniformBlockDesc* _desc );

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		/// TODO: remove?
		cShaderProgram* m_activeProgram = nullptr;
		RenderTarget* m_activeRenderTarget = nullptr;

		int m_numTotalUniformBlocks = 0;

		// states
		std::vector<wv::Handle> m_boundTextureSlots;
		wv::Handle m_boundUniformBuffer = 0;

	};

	template<typename ...Args>
	inline bool cOpenGLGraphicsDevice::assertGLError( const std::string _msg, Args ..._args )
	{
		std::string error;
		if ( !getError( &error ) )
			return false;
		
		Debug::Print( Debug::WV_PRINT_ERROR, _msg.c_str(), _args... );
		Debug::Print( Debug::WV_PRINT_ERROR, "  %s\n", error.c_str() );

		return false;
	}
}