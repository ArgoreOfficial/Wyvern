#pragma once
#include "Wyvern/Core/iLayer.h"
#include "Wyvern/cApplication.h"
#include "Wyvern/Logging/cLogging.h"
#include "Wyvern/Managers/cEventManager.h"
#include "Wyvern/Renderer/cModel.h"
#include "Wyvern/Renderer/Camera/cCamera.h"

class GameLayer : public wv::iLayer
{
public:
	GameLayer() :
		iLayer( "GameLayer" ), 
		m_scene( wv::cApplication::getScene() )
	{
	};

	~GameLayer() { };

	void start() override;
	void update( double _deltaTime ) override;
	void draw3D() override { }
	void draw2D() override { }
	void drawUI() override;

	void handleKeyInput( wv::Events::cKeyDownEvent& _event );

	wv::cSceneGraph& m_scene;
	wv::cMesh* m_meshAsset = nullptr;
	wv::cCamera m_camera;
};

