#pragma once
#include <Wyvern/Game.h>
#include <Wyvern/Rendering/Renderer.h>
#include <Wyvern/Logging/Logging.h>

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
