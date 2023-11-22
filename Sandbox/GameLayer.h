#pragma once

#include <Wyvern/Core/iLayer.h>
#include <Wyvern/cApplication.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Managers/cEventManager.h>
#include <Wyvern/Renderer/cModel.h>
#include <Wyvern/Renderer/Camera/cCamera.h>

///////////////////////////////////////////////////////////////////////////////////////

class GameLayer : public wv::iLayer
{

public:

	GameLayer( void ) :
		iLayer( "GameLayer" ), 
		m_scene( wv::cApplication::getScene() )
	{
	};

	~GameLayer( void ) { };

///////////////////////////////////////////////////////////////////////////////////////

	void start ( void )              override;
	void update( double _deltaTime ) override;
	void draw3D( void )              override { }
	void draw2D( void )              override { }
	void drawUI( void )              override;
	void handleKeyInput( wv::Events::cKeyDownEvent& _event );

///////////////////////////////////////////////////////////////////////////////////////

	wv::cSceneGraph& m_scene;
	wv::cCamera      m_camera;

};

