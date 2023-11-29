#pragma once

#include <Wyvern/Core/iLayer.h>
#include <Wyvern/cApplication.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Managers/cEventManager.h>
#include <Wyvern/Renderer/Camera/cCamera.h>
#include <Wyvern/Assets/cModel.h>

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
	void draw3D( void )              override;
	void draw2D( void )              override { }
	void drawUI( void )              override;
	void handleKeyDownInput( wv::Events::cKeyDownEvent& _event );
	void handleKeyUpInput( wv::Events::cKeyUpEvent& _event );

///////////////////////////////////////////////////////////////////////////////////////

	wv::cSceneGraph& m_scene;
	wv::cCamera*     m_camera;
	wv::cModel m_model{ "assets/meshes/logo.obj" };
	float dx = 0.0f;
	float dz = 0.0f;
	float dr = 0.0f;
};

