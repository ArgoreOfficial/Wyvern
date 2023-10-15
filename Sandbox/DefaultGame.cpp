#include "DefaultGame.h"
#include <Wyvern/Managers/AssetManager.h>
// #include <Wyvern/Rendering/Renderer.h>
#include <Wyvern/Events/Events.h>

void DefaultGame::load()
{

	WV::AssetManager::load<WV::Mesh>( "meshes/logo.obj" );
	WV::AssetManager::load<WV::Mesh>( "meshes/monitor_bare.obj" );
	WV::AssetManager::load<WV::Mesh>( "meshes/monitor_screen.obj" );

	WV::AssetManager::load<WV::Texture>( "textures/logo_fire_BaseColor.png" );
	WV::AssetManager::load<WV::Texture>( "textures/reaction_sheet.png" );
	WV::AssetManager::load<WV::Texture>( "textures/monitor.png" );
	WV::AssetManager::load<WV::Texture>( "textures/point_and_laugh.gif" );
}

void keyEventHandler( WV::Events::KeyDownEvent& _event )
{
	// WVDEBUG( "keycode %c was pressed", (char)_event.getKeyCode() );
}

void DefaultGame::start()
{
	/*
	m_camera = new WV::Camera( 40, 800, 800 );
	WV::Renderer::setActiveCamera( m_camera );

	m_camera->rotate( glm::vec3( 0.0f, 0.0f, 0.0f ) );
	m_camera->move( glm::vec3( 0, 0.0f, 3.0f ) );

	m_logo = WV::AssetManager::assembleModel( "logo.obj", "logo_fire_BaseColor.png" );

	m_monitor = WV::AssetManager::assembleModel( "monitor_bare.obj", "monitor.png" );
	m_monitorScreen = WV::AssetManager::assembleModel( "monitor_screen.obj", "point_and_laugh.gif" );
	// m_monitorScreen->getMaterial().setTileCount( { 109, 1 } );

	glm::vec3 pos( 0.25f, -0.2f, 1.5f );
	m_monitor->setPosition( pos );
	m_monitorScreen->setPosition( pos );

	m_monitor->getMesh().recalculateNormals();

	WV::Events::KeyDownEvent::hook( keyEventHandler );
	*/
}

void DefaultGame::update( float _deltaTime )
{
	float time = WV::Application::getTime();

	m_logo->setPosition( { -0.5f, sinf( time ) / 15.0f, 0 } );
	m_logo->rotate( { 0, _deltaTime * 45, 0 } );

	glm::vec3 monitorRotation = 
	{ 
		cosf( time ) * 15.0f, 
		sinf( time ) * 15.0f, 
		0 
	};

	m_monitor->setRotation( monitorRotation );
	m_monitorScreen->setRotation( monitorRotation );

	// m_monitorScreen->getMaterial().setUVOffset( { floorf( time * 28.0f ), 0 } );
}

void DefaultGame::draw()
{
	WV::Renderer::getInstance().draw( *m_logo );

	WV::Renderer::getInstance().draw( *m_monitor );
	WV::Renderer::getInstance().draw( *m_monitorScreen );
}