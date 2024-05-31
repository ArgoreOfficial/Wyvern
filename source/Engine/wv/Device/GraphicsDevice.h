#pragma once
#include <wv/Types.h>
#include <wv/Shader/Shader.h>
#include <wv/Shader/ShaderProgram.h>

#include <wv/Misc/Color.h>

namespace wv
{
	struct PipelineDesc;
	class Pipeline;

	struct PrimitiveDesc;
	class Primitive;
	
	struct TextureDesc;
	class Texture;

	struct UniformBlockDesc;

	struct RenderTargetDesc;
	class RenderTarget;

	struct GraphicsDeviceDesc
	{
		GraphicsDriverLoadProc loadProc;
		GraphicsAPI graphicsApi;
	};

	class GraphicsDevice /// TODO: make IGraphicsDevice?
	{
	public:

		static inline GraphicsDevice* createGraphicsDevice( GraphicsDeviceDesc* _desc )
		{
			/// TODO: different platforms

			return new GraphicsDevice( _desc );
		}

		void terminate();

		void onResize( int _width, int _height );

		RenderTarget* createRenderTarget( RenderTargetDesc* _desc );
		void destroyRenderTarget( RenderTarget** _renderTarget );

		void setRenderTarget( RenderTarget* _target );
		void clearRenderTarget( const wv::Color& _color );

		Pipeline* createPipeline( PipelineDesc* _desc );
		void destroyPipeline( Pipeline** _pipeline );
		void setActivePipeline( Pipeline* _pipeline );

		Primitive* createPrimitive( PrimitiveDesc* _desc );
		Texture* createTexture( TextureDesc* _desc );
		Texture* temporaryCreateTexture( TextureDesc* _desc, int _width, int _height);
		void bindTextureToSlot( Texture* _texture, unsigned int _slot );

		void draw( Primitive* _primitive );

	private:
		/// TODO: change wv::Handle to wv::Shader and wv::ShaderProgram?
		wv::Handle createShader( ShaderSource* _desc );
		wv::Handle createProgram( ShaderProgramDesc* _desc );

		void createUniformBlock( Pipeline* _pipeline, UniformBlockDesc* _desc );

		GraphicsDevice( GraphicsDeviceDesc* _desc );

		wv::Pipeline* m_activePipeline = nullptr;
		int m_numTotalUniformBlocks = 0;
	};
}