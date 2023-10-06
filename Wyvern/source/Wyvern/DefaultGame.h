#pragma once
#include <Wyvern/Game.h>

namespace WV
{
	class WYVERN_API DefaultGame : public Game
	{
	public:
		DefaultGame();
		~DefaultGame();

		void update( float _deltaTime ) override;
		void draw() override;
	};
}
