#pragma once
#include <Wyvern/API/Core.h>

namespace WV
{
	class WYVERN_API Game
	{
	public:
		Game();
		~Game();

		virtual void load() { }
		virtual void update( float _deltaTime ) { }
		virtual void draw() { }

	private:
		bool m_running = true;
	};
}
