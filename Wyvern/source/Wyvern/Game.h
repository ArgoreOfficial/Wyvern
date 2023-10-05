#pragma once

namespace WV
{
	class Game
	{
	public:
		Game();
		~Game();

		virtual void run() { }
		virtual void update( float _deltaTime ) { }
		virtual void draw() { }
	};
}
