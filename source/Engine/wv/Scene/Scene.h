#pragma once

#include <wv/Decl.h>

namespace wv
{
	class GraphicsDevice;

	class Scene
	{
	WV_DECLARE_INTERFACE( Scene )

	public:

		virtual void onLoad() = 0;
		virtual void onUnload() = 0;

		virtual void onCreate() = 0;
		virtual void onDestroy() = 0;

		virtual void update( double _deltaTime ) = 0;
		virtual void draw( GraphicsDevice* _device ) = 0;

	protected:



	};
}