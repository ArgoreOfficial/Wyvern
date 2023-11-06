#pragma once
#include "Wyvern/Core/ILayer.h"
#include "Wyvern/Application.h"
#include "Wyvern/Logging/Logging.h"
#include "Wyvern/Managers/EventManager.h"
#include "Wyvern/Rendering/Model.h"
#include "Wyvern/Camera.h"

class GameLayer : public WV::ILayer
{
public:
	GameLayer() : 
		ILayer( "GameLayer" ), m_scene( WV::Application::getScene() ) 
	{ };

	~GameLayer() { };

	void start() override;
	void update( double _deltaTime ) override;
	void drawUI() override;

	void handleKeyInput( WV::Events::KeyDownEvent& _event );

	WV::SceneGraph& m_scene;
	WV::Mesh* m_meshAsset = nullptr;
	WV::Camera m_camera;
};

