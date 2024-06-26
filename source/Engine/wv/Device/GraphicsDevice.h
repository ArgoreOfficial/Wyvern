#pragma once

#include <wv/Types.h>
#include <wv/Shader/Shader.h>
#include <wv/Shader/ShaderProgram.h>
#include <wv/Misc/Color.h>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct PipelineDesc;
	struct PrimitiveDesc;
	struct TextureDesc;
	struct UniformBlockDesc;
	struct RenderTargetDesc;
	struct MeshDesc;

	class Primitive;
	class Texture;
	class RenderTarget;
	class Mesh;

	class cShader;
	
///////////////////////////////////////////////////////////////////////////////////////

	struct GraphicsDeviceDesc
	{
		GraphicsDriverLoadProc loadProc;
		GraphicsAPI graphicsApi;
		GenericVersion graphicsApiVersion;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class iGraphicsDevice
	{
	public:

		static inline iGraphicsDevice* createGraphicsDevice( GraphicsDeviceDesc* _desc )
		{
			/// TODO: different backends

			return new iGraphicsDevice( _desc );
		}

		void terminate();

		void onResize( int _width, int _height );
		void setViewport( int _width, int _height );

		RenderTarget* createRenderTarget( RenderTargetDesc* _desc );
		void destroyRenderTarget( RenderTarget** _renderTarget );

		void setRenderTarget( RenderTarget* _target );
		void setClearColor( const wv::cColor& _color );
		void clearRenderTarget( bool _color, bool _depth );

		cShader* createShader( eShaderType _type, const std::string& _name );
		void destroyShader( cShader* _shader );
		void compileShader( cShader* _shader );

		cShaderProgram* createProgram( const std::string& _name );
		void destroyProgram( cShaderProgram* _program );
		void linkProgram( cShaderProgram* _program, std::vector<UniformBlockDesc> _uniformBlocks = {}, std::vector<Uniform> _textureUniforms = { } );
		void useProgram( cShaderProgram* _program );


		Mesh* createMesh( MeshDesc* _desc );
		void destroyMesh( Mesh** _mesh );

		Primitive* createPrimitive( PrimitiveDesc* _desc, Mesh* _mesh );
		void destroyPrimitive( Primitive** _primitive );

		Texture* createTexture( TextureDesc* _desc );
		void destroyTexture( Texture** _texture );

		void bindTextureToSlot( Texture* _texture, unsigned int _slot );

		void draw( Mesh* _mesh );

///////////////////////////////////////////////////////////////////////////////////////

	private:

		template<typename... Args>
		bool assertGLError( const std::string _msg, Args..._args );
		bool getError( std::string* _out );

		void drawPrimitive( Primitive* _primitive );
		UniformBlock createUniformBlock( cShaderProgram* _program, UniformBlockDesc* _desc );

		iGraphicsDevice( GraphicsDeviceDesc* _desc );

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		/// TODO: remove?
		cShaderProgram* m_activeProgram = nullptr;
		int m_numTotalUniformBlocks = 0;
	};

	template<typename ...Args>
	inline bool iGraphicsDevice::assertGLError( const std::string _msg, Args ..._args )
	{
		std::string error;
		if ( !getError( &error ) )
			return false;
		
		Debug::Print( Debug::WV_PRINT_ERROR, _msg.c_str(), _args... );
		Debug::Print( Debug::WV_PRINT_ERROR, "  %s\n", error.c_str() );

		return false;
	}
}