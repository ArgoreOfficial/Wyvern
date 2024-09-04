#pragma once

#include <wv/Types.h>

#include <wv/Shader/Shader.h>
#include <wv/Shader/ShaderProgram.h>

#include <wv/Misc/Color.h>

#include <wv/Graphics/GPUBuffer.h>

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
	
	struct iDeviceContext;

	class Primitive;
	class Texture;
	class RenderTarget;
	class Mesh;

	class cShader;
	class cShaderProgram;
	
///////////////////////////////////////////////////////////////////////////////////////

	struct GraphicsDeviceDesc
	{
		GraphicsDriverLoadProc loadProc;
		iDeviceContext* pContext;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class iGraphicsDevice
	{
	public:

		virtual ~iGraphicsDevice() = 0;

		static iGraphicsDevice* createGraphicsDevice( GraphicsDeviceDesc* _desc );

		virtual void terminate() = 0;

		virtual void onResize( int _width, int _height ) = 0;
		virtual void setViewport( int _width, int _height ) = 0;

		virtual RenderTarget* createRenderTarget( RenderTargetDesc* _desc ) = 0;
		virtual void destroyRenderTarget( RenderTarget** _renderTarget ) = 0;

		virtual void setRenderTarget( RenderTarget* _target ) = 0;
		virtual void setClearColor( const wv::cColor& _color ) = 0;
		virtual void clearRenderTarget( bool _color, bool _depth ) = 0;

		virtual bool createShader( cShader* _shader, eShaderType _type ) = 0;
		virtual void destroyShader( cShader* _shader ) = 0;
		virtual void compileShader( cShader* _shader ) = 0;

		virtual void createProgram( wv::cShaderProgram* _program, const std::string& _name ) = 0;
		virtual void destroyProgram( cShaderProgram* _program ) = 0;
		virtual void linkProgram( cShaderProgram* _program, std::vector<UniformBlockDesc> _uniformBlocks = { }, std::vector<Uniform> _textureUniforms = { } ) = 0;
		virtual void useProgram( cShaderProgram* _program ) = 0;

		virtual sGPUBuffer* createGPUBuffer( eGPUBufferType _type ) = 0;
		virtual void bufferData( sGPUBuffer* _buffer, void* _data, size_t _size ) = 0;
		virtual void destroyGPUBuffer( sGPUBuffer* _buffer ) = 0;
		
		virtual Mesh* createMesh( MeshDesc* _desc ) = 0;
		virtual void destroyMesh( Mesh** _mesh ) = 0;

		virtual Primitive* createPrimitive( PrimitiveDesc* _desc, Mesh* _mesh ) = 0;
		virtual void destroyPrimitive( Primitive** _primitive ) = 0;

		virtual void createTexture( Texture* _pTexture, TextureDesc* _desc ) = 0;
		virtual void destroyTexture( Texture** _texture ) = 0;

		virtual void bindTextureToSlot( Texture* _texture, unsigned int _slot ) = 0;

		virtual void draw( Mesh* _mesh ) = 0;

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		iGraphicsDevice() { };

		virtual bool initialize( GraphicsDeviceDesc* _desc ) = 0;

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		
	};
}