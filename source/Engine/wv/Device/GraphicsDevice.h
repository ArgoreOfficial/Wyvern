#pragma once
#include <wv/Types.h>
#include <wv/Shader/Shader.h>
#include <wv/Shader/ShaderProgram.h>

namespace wv
{
	struct PipelineDesc;
	class Pipeline;
	struct PrimitiveDesc;
	class Primitive;
	
	struct UniformBlockDesc;

	struct GraphicsDeviceDesc
	{
		GraphicsDriverLoadProc loadProc;
		GraphicsAPI graphicsApi;
	};

	/// <summary>
	/// TEMPORARY
	/// TODO: ADD RENDER TARGET
	/// </summary>
	struct DummyRenderTarget
	{
		int framebuffer;
		int width;
		int height;
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

		void setRenderTarget( DummyRenderTarget* _target );
		void clearRenderTarget( const float _color[4] );

		wv::Pipeline* createPipeline( PipelineDesc* _desc );
		void destroyPipeline( Pipeline** _pipeline );
		void setActivePipeline( Pipeline* _pipeline );

		Primitive* createPrimitive( PrimitiveDesc* _desc );

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