#pragma once
#include <wv/Types.h>

namespace wv
{
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

		void setRenderTarget( DummyRenderTarget* _target );
		void clearRenderTarget( const float _color[4] );

	private:

		GraphicsDevice( GraphicsDeviceDesc* _desc );

	};
}