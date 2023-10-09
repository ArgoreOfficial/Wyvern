#pragma once
#include <Wyvern/Game.h>
#include <Wyvern/Rendering/Renderer.h>
#include <Wyvern/Logging/Logging.h>

class DefaultGame : public WV::Game
{
public:
	DefaultGame() { };
	~DefaultGame() { };

	void load() override;
	void update( float _deltaTime ) override;
	void draw() override;

	WV::Camera* m_camera = nullptr;
	WV::RenderObject* m_testModel = nullptr;
	WV::RenderObject* m_testModel2 = nullptr;
};

