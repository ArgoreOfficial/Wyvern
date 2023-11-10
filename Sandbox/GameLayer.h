#pragma once
#include "Wyvern/Core/iLayer.h"
#include "Wyvern/cApplication.h"
#include "Wyvern/Logging/cLogging.h"
#include "Wyvern/Managers/cEventManager.h"
#include "Wyvern/Rendering/cModel.h"
#include "Wyvern/cCamera.h"

class GameLayer : public WV::iLayer
{
public:
	GameLayer() : 
		iLayer( "GameLayer" ), m_scene( WV::cApplication::getScene() ) 
	{ };

	~GameLayer() { };

	void start() override;
	void update( double _deltaTime ) override;
	void drawUI() override;

	void handleKeyInput( WV::Events::cKeyDownEvent& _event );

	WV::cSceneGraph& m_scene;
	WV::cMesh* m_meshAsset = nullptr;
	WV::cCamera m_camera;
};

