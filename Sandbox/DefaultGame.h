#pragma once
#include <Wyvern/Core/ILayer.h>
#include <Wyvern/Application.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Managers/EventManager.h>
#include <Wyvern/Rendering/Model.h>
#include <Wyvern/Camera.h>

class DefaultGame : public WV::ILayer
{
public:
	DefaultGame() { };
	~DefaultGame() { };

	void start() override;
	void update( double _deltaTime ) override;
	void draw3D() override;
	void drawUI() override;

	void handleKeyInput( WV::Events::KeyDownEvent& _event );

	float m_slider = 0.0f;
	bool m_showWindow = false;

	WV::Model* m_model;
	WV::Camera m_camera;
};

