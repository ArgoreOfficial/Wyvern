#pragma once
#include <Wyvern/Game.h>
#include <Wyvern/Application.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Managers/EventManager.h>

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

	bgfx::VertexLayout layout;
	bgfx::VertexBufferHandle vbh;
	bgfx::IndexBufferHandle ibh;
	bgfx::ShaderHandle vsh;
	bgfx::ShaderHandle fsh;
	bgfx::ProgramHandle program;

};

