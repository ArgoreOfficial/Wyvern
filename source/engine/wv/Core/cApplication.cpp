#include "cApplication.h"

#include <cm/Backends/iBackend.h>
#include <cm/Core/cWindow.h>

#include <wv/Camera/iCamera.h>
#include <wv/Camera/cFreeflightCamera.h>

#include <wv/Core/cRenderer.h>

#include <wv/Manager/cContentManager.h>

#include <wv/Scene/cSceneLoader.h>
#include <wv/Scene/cSceneManager.h>

#include <format>

wv::cApplication::cApplication() :
	m_window  { new cm::cWindow() }
{
}

wv::cApplication::~cApplication()
{
	m_window->destroy();

	delete m_camera2D;
	delete m_camera3D;
	m_current_camera = nullptr;
	m_camera2D = nullptr;
	m_camera3D = nullptr;
}

void wv::cApplication::create()
{

	m_window->create( 1500, 1000, "Wyvern" );
	m_window->setVSync( false );

	cRenderer      ::getInstance().create();
	cContentManager::getInstance().create();
	cSceneManager  ::getInstance().create();

	m_camera2D = new cFreeflightCamera( iCamera::CameraType_Orthographic );
	m_camera3D = new cFreeflightCamera( iCamera::CameraType_Perspective );

}

void wv::cApplication::onResize( int _width, int _height )
{
	cRenderer::getInstance().onResize( _width, _height );
	m_window->onResize( _width, _height );
}

void wv::cApplication::onRawInput( sInputInfo* _info )
{
	m_camera3D->onRawInput( _info );

	if ( _info->buttondown )
	{
		int debug_render_mode = -1;

		switch ( _info->key )
		{
		case GLFW_KEY_1: debug_render_mode = 1; break;
		case GLFW_KEY_2: debug_render_mode = 2; break;
		case GLFW_KEY_3: debug_render_mode = 3; break;
		case GLFW_KEY_4: debug_render_mode = 4; break;
		case GLFW_KEY_5: debug_render_mode = 5; break;
		}

		if ( debug_render_mode != -1 )
			cRenderer::getInstance().debug_render_mode = debug_render_mode;
	}
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

	m_camera3D->getTransform().position = { 0.0f, 0.0f, 0.0f };
	m_current_camera = m_camera3D;

	while ( !m_window->shouldClose() )
	{
		updateDeltaTime( time, delta_time );

		m_window->setTitle( std::format( "FPS: {}", ( 1.0 / m_average_frametime ) ).c_str() );
		m_window->processInput();

		m_camera2D->update( delta_time );
		m_camera3D->update( delta_time );

		scene_manager.update( delta_time );
		
		renderer.begin();
		renderer.clear( 0x00000000, cm::ClearMode_Color | cm::ClearMode_Depth );

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

	m_frametime_buffer[ m_frametime_buffer_counter ] = _delta_time;
	m_frametime_buffer_counter++;
	if ( m_frametime_buffer_counter >= AVERAGE_FRAMETIME_BUFFER_SIZE )
		m_frametime_buffer_counter = 0;

	for ( int i = 0; i < AVERAGE_FRAMETIME_BUFFER_SIZE; i++ )
		m_average_frametime += m_frametime_buffer[ i ];
	
	m_average_frametime /= (double)AVERAGE_FRAMETIME_BUFFER_SIZE;
}

