#pragma once
#include <Wyvern/Game.h>
#include <Wyvern/Application.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Managers/EventManager.h>
#include <Wyvern/Rendering/Model.h>
#include <Wyvern/Camera.h>

class DefaultGame : public WV::Game
{
public:
	DefaultGame() { };
	~DefaultGame() { };

	void load() override;
	void start() override;
	void update( float _deltaTime ) override;
	void draw() override;
	void drawUI() override;

	void handleKeyInput( WV::Events::KeyDownEvent& _event );

	float m_slider = 0.0f;
	bool m_showWindow = false;

	WV::Model* m_model;
	WV::Camera m_camera;
};

