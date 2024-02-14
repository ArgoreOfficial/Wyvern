#include "cApplication.h"

#include <cm/Backends/iBackend.h>
#include <cm/Core/cWindow.h>

#include <wv/Camera/cCamera.h>
#include <wv/Core/cRenderer.h>

#include <wv/Graphics/cSprite.h>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>
#include <wv/Manager/cContentManager.h>

#include <wv/Scene/cScene.h>
#include <wv/Scene/cSceneLoader.h>
#include <wv/Scene/cSceneManager.h>

#include <wv/Graphics/cModel.h>

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

	cRenderer      ::getInstance().create();
	cContentManager::getInstance().create();
	cSceneManager  ::getInstance().create();

	m_camera2D = new cCamera( cCamera::CameraType_Orthographic );
	m_camera3D = new cCamera( cCamera::CameraType_Perspective );

}

void wv::cApplication::onResize( int _width, int _height )
{
	cRenderer::getInstance().onResize( _width, _height );
	m_window->onResize( _width, _height );
}

void wv::cApplication::run( cSceneLoader* _scene_loader )
{
	cSceneManager& scene_manager = cSceneManager::getInstance();
	cRenderer& renderer          = cRenderer    ::getInstance();

	double time = m_window->getTime();
	double delta_time = 0.0;
	
	scene_manager.createScene( _scene_loader->getName() );
	scene_manager.switchScene( _scene_loader->getName() );
	scene_manager.update( 1.0 );
	scene_manager.loadScene( _scene_loader );

	m_camera3D->getTransform().position = { 0.0f, 0.0f, 2.0f };
	m_current_camera = m_camera3D;

	while ( !m_window->shouldClose() )
	{
		updateDeltaTime( time, delta_time );

		m_window->setTitle( std::format( "FPS: {}", ( 1.0 / delta_time ) ).c_str() );
		m_window->processInput();

		m_camera3D->getTransform().position.z = ( sin( time ) + 1.0f ) * 1.5f + 0.5f;

		scene_manager.update( delta_time );
		
		renderer.begin();
		renderer.clear( 0x000000FF );

		scene_manager.render();

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

