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

	class deprecated_Pipeline;
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

		deprecated_Pipeline* createPipeline( PipelineDesc* _desc );
		void destroyPipeline( deprecated_Pipeline** _pipeline );
		deprecated_Pipeline* getPipeline( const char* _name );

		cShader* createShader( eShaderType _type );
		void compileShader( cShader* _shader );

		cShaderProgram* createProgram();
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

		void drawPrimitive( Primitive* _primitive );
		UniformBlock createUniformBlock( cShaderProgram* _program, UniformBlockDesc* _desc );

		iGraphicsDevice( GraphicsDeviceDesc* _desc );

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		wv::deprecated_Pipeline* m_activePipeline = nullptr;
		int m_numTotalUniformBlocks = 0;

		std::unordered_map<std::string, wv::deprecated_Pipeline*> m_pipelines;

	};
}