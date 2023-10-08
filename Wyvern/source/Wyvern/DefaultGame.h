#pragma once
#include <Wyvern/Game.h>

namespace WV
{
	class DefaultGame : public Game
	{
	public:
		DefaultGame();
		~DefaultGame();

		void update( float _deltaTime ) override;
		void draw() override;
	};
}
