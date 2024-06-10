#pragma once

#include <wv/Decl.h>

namespace wv
{
	class Scene;

	class State
	{
	WV_DECLARE_INTERFACE(State)

	public:

		/*
		
		onLoad -> onCreate -> loop(   update -> draw   ) -> onDestroy -> onUnload
		
		*/

		virtual void onLoad()   = 0;
		virtual void onUnload() = 0;

		virtual void onCreate()   = 0;
		virtual void onDestroy() = 0;

		virtual void update( double _deltaTime ) = 0;
		virtual void draw() = 0;

		inline void switchToScene( Scene* _scene )
		{
			m_currentScene = _scene; /// TODO: multithread
		}

	protected:

		Scene* m_currentScene = nullptr;

	};
}