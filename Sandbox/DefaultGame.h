#pragma once
#include <Wyvern/Game.h>
#include <Wyvern/Application.h>
#include <Wyvern/Logging/Logging.h>

class DefaultGame : public WV::Game
{
public:
	DefaultGame() { };
	~DefaultGame() { };

	void load() override;
	void start() override;
	void update( float _deltaTime ) override;
	void draw() override;
};

