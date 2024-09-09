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
	struct sShaderBufferDesc;
	struct RenderTargetDesc;
	struct MeshDesc;
	
	struct iDeviceContext;

	class Primitive;
	class Texture;
	class RenderTarget;
	class Mesh;
	struct sMeshNode;

	struct sShaderProgramSource;
	struct sShaderProgram;

	struct sPipelineDesc;
	struct sPipeline;
	
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

		virtual ~iGraphicsDevice() { };

		static iGraphicsDevice* createGraphicsDevice( GraphicsDeviceDesc* _desc );

		virtual void terminate() = 0;

		virtual void onResize( int _width, int _height ) = 0;
		virtual void setViewport( int _width, int _height ) = 0;

		virtual void beginRender() {}
		virtual void endRender  () {}

		virtual RenderTarget* createRenderTarget( RenderTargetDesc* _desc ) = 0;
		virtual void destroyRenderTarget( RenderTarget** _renderTarget ) = 0;

		virtual void setRenderTarget( RenderTarget* _target ) = 0;
		virtual void setClearColor( const wv::cColor& _color ) = 0;
		virtual void clearRenderTarget( bool _color, bool _depth ) = 0;

		virtual sShaderProgram* createProgram( eShaderProgramType _type, sShaderProgramSource* _source ) = 0;
		virtual void destroyProgram( sShaderProgram* _shader ) = 0;

		virtual sPipeline* createPipeline( sPipelineDesc* _desc ) = 0;
		virtual void destroyPipeline( sPipeline* _program ) = 0;
		virtual void bindPipeline   ( sPipeline* _program ) = 0;

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

		virtual void drawPrimitive( Primitive* _primitive ) = 0;

		void draw( Mesh* _mesh );
		void drawNode( sMeshNode* _node );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		iGraphicsDevice() { };

		virtual bool initialize( GraphicsDeviceDesc* _desc ) = 0;

		GraphicsAPI    m_graphicsApi;
		GenericVersion m_graphicsApiVersion;

		
	};
}