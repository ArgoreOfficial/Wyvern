#pragma once
#include <Wyvern/Game.h>
#include <Wyvern/Application.h>
#include <Wyvern/Logging/Logging.h>

namespace WV { class Camera; }
namespace WV { class Model; }

class DefaultGame : public WV::Game
{
public:
	DefaultGame() { };
	~DefaultGame() { };

	void load() override;
	void start() override;
	void update( float _deltaTime ) override;
	void draw() override;

	WV::Camera* m_camera = nullptr;
	WV::Model* m_logo = nullptr;
	
	WV::Model* m_monitor = nullptr;
	WV::Model* m_monitorScreen = nullptr;
};

