#include "cApplication.h"

#include <cm/Backends/iBackend.h>
#include <cm/Core/cWindow.h>

#include <wv/Camera/iCamera.h>
#include <wv/Camera/cFreeflightCamera.h>

#include <wv/Core/cRenderer.h>
#include <wv/Managers/cContentManager.h>
#include <wv/Scene/cSceneManager.h>

#include <format>

wv::cApplication::cApplication() :
	m_window  { new cm::cWindow() }
{
}

wv::cApplication::~cApplication()
{
	m_window->destroy();
	delete m_window;

	delete m_camera2D;
	delete m_camera3D;

	m_current_camera = nullptr;
	m_camera2D = nullptr;
	m_camera3D = nullptr;
}

void wv::cApplication::create()
{
	//m_window->setWindowAttribute( cm::WindowAttribute_Maximized, true );
	m_window->setWindowAttribute( cm::WindowAttribute_Focused, true );

	m_window->create( 1500, 1000, "Wyvern" );
	m_window->setVSync( false );


	cRenderer      ::getInstance().create();
	cContentManager::getInstance().create();
	cSceneManager  ::getInstance().create();

	m_camera2D = new cFreeflightCamera( iCamera::CameraType_Orthographic );
	m_camera3D = new cFreeflightCamera( iCamera::CameraType_Perspective );

	m_camera2D->onCreate();
	m_camera3D->onCreate();

	onResize( m_window->getWidth(), m_window->getHeight() );

	subscribeInputEvent();
}

void wv::cApplication::onResize( int _width, int _height )
{
	m_window->onResize( _width, _height );
	
	cRenderer::getInstance().onResize( _width, _height );
#if defined( WV_RELEASE )
#endif
}

void wv::cApplication::run( iApplicationConfig* _config_creator )
{
	m_config_creator = _config_creator;
	m_config = _config_creator->config();

	double time = m_window->getTime();
	double delta_time = 0.0;
	
	init();

	while ( !m_window->shouldClose() )
	{
		updateDeltaTime( time, delta_time );
		
		update( delta_time );
		render();
	}
}

void wv::cApplication::onInputEvent( sInputEvent _event )
{
	if ( _event.buttondown )
	{
		int debug_render_mode = cRenderer::getInstance().debug_render_mode;

		switch ( _event.key )
		{
		case GLFW_KEY_F5: cContentManager::getInstance().reloadAllShaders(); break;
		}
	}
}

void wv::cApplication::init( void )
{
	cSceneManager& scene_manager = cSceneManager::getInstance();
	scene_manager.createScene( "GameScene" ); // TODO: change to sceneloader
	scene_manager.switchScene( "GameScene" );
	scene_manager.update( 1.0 );
	//scene_manager.loadScene( _scene_loader );

	m_camera3D->getTransform().position = { 0.0f, 0.0f, 1.0f };
	m_current_camera = m_camera3D;

#ifdef WV_DEBUG
	m_config_creator->debugInit();
#endif
}

void wv::cApplication::update( double _delta_time )
{
	cSceneManager& scene_manager = cSceneManager::getInstance();

	// update
	m_window->setTitle( std::format( "FPS: {}", ( 1.0 / m_average_frametime ) ).c_str() );

	m_window->processInput();

#ifdef WV_DEBUG
	m_config_creator->debugUpdate( _delta_time );
#endif

	m_camera2D->update( _delta_time );
	m_camera3D->update( _delta_time );

	scene_manager.update( _delta_time );
}

void wv::cApplication::render( void )
{
	cRenderer& renderer = cRenderer::getInstance();
	cSceneManager& scene_manager = cSceneManager::getInstance();

	renderer.begin();
	renderer.clear( 0x00000000, cm::ClearMode_Color | cm::ClearMode_Depth );

	scene_manager.render();

#ifdef WV_DEBUG
	m_config_creator->debugRender();
#endif

	renderer.end();

#if defined( WV_DEBUG )
	m_config_creator->debugViewbufferBegin();
#endif

	renderer.display();

#if defined( WV_DEBUG )
	m_config_creator->debugViewbufferEnd();
#endif

	m_window->display();
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

