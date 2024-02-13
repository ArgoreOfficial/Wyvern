#include "cApplication.h"

#include <wv/Camera/cCamera.h>

#include <cm/Core/cWindow.h>
#include <cm/Backends/iBackend.h>

#include <wv/Core/cRenderer.h>
#include <wv/Graphics/cSprite.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>
#include <wv/Manager/cContentManager.h>

#define STB_IMAGE_IMPLEMENTATION
#include <cm/Core/stb_image.h>





#include <format>





wv::cApplication::cApplication() :
	m_window  { new cm::cWindow() }
{
}

wv::cApplication::~cApplication()
{
	m_window->destroy();
}

void wv::cApplication::create()
{

	m_window->create( 1920, 1080, "renderer idfk" );
	m_window->setVSync( false );

	wv::cRenderer::getInstance().create();

	wv::cContentManager::getInstance();

	m_camera2D = new wv::cCamera( cCamera::CameraType_Orthographic );
	m_camera3D = new wv::cCamera( cCamera::CameraType_Perspective );

}

void wv::cApplication::onResize( int _width, int _height )
{
	wv::cRenderer::getInstance().onResize( _width, _height );
	m_window->onResize( _width, _height );
}

void wv::cApplication::onRawInput( sInputInfo* _info )
{
	// change to event manager?
}

void wv::cApplication::run()
{
	double time = m_window->getTime();;
	double delta_time = 0.0;
	

	/* sandbox */
	wv::cSprite sprite;
	sprite.create( "../res/textures/wyvern_logo_white.png" );

	sprite .getTransform().position = { 0.0f, 0.0f, 0.0f };
	sprite .getTransform().scale    = { 1.0f, 1.0f, 1.0f };

	
	// ground sprite
	wv::cSprite sprite2;
	sprite2.create( "../res/textures/funnyman.png" );
	
	sprite2.getTransform().position = {  0.0f,  -1.0f,  0.0f };
	sprite2.getTransform().rotation = { -90.0f,  0.0f,  0.0f };
	sprite2.getTransform().scale    = {  10.0f, 10.0f, 10.0f };


	m_camera3D->getTransform().position = { 0.0f, 0.0f, 2.0f };
	m_current_camera = m_camera3D;

	/* sandbox */

	wv::cRenderer& renderer = wv::cRenderer::getInstance();


	while ( !m_window->shouldClose() )
	{
		updateDeltaTime( time, delta_time );

		m_window->setTitle( std::format( "FPS: {}", ( 1.0 / delta_time ) ).c_str() );
		m_window->processInput();

		// scenemanager.update();
		
		m_camera3D->getTransform().position.z = (sin( time ) + 1.0f) * 1.5f + 0.5f;
		
		sprite.getTransform().rotate( { 90.0f * (float)delta_time, 180.0f * (float)delta_time, 90.0f * (float)delta_time } );

		renderer.begin();
		renderer.clear( 0x000000FF );

		sprite2.render();
		sprite.render();

		renderer.end();
		m_window->display();
	}
}

void wv::cApplication::updateDeltaTime( double& _time, double& _delta_time )
{
	double now = m_window->getTime();
	_delta_time = now - _time;
	_time = now;
}

