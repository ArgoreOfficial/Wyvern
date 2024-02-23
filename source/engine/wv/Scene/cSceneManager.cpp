#include "cSceneManager.h"

#include <wv/Scene/cScene.h>
#include <wv/Scene/cSceneLoader.h>

wv::cSceneManager::cSceneManager( void )
{

}

wv::cSceneManager::~cSceneManager( void )
{
	for ( auto& scene : m_scenes )
		delete scene.second;
	
	m_scenes.clear();
}

void wv::cSceneManager::create( void )
{
	light_positions.push_back( { 0.0f, 1.0f, 0.0f, 1.0f } );
	light_colors.push_back( { 1.0f, 0.9f, 0.5f, 5.8f } );
}

void wv::cSceneManager::update( double _delta_time )
{
	if ( m_active_scene )
		m_active_scene->update( _delta_time );

	if ( !m_next_scene )
		return;

	m_transition_time += _delta_time;

	if ( m_transition_time >= m_transition_length )
	{
		m_active_scene->unload(); // unload old scene
		m_active_scene = m_next_scene; // switch to new scene
		m_active_scene->load(); // load new scene
		m_next_scene = nullptr;
	}

}

void wv::cSceneManager::render( void )
{
	m_active_scene->render();
}

void wv::cSceneManager::createScene( std::string _name )
{
	if ( m_scenes.count( _name ) != 0 )
	{
		printf( "[ERROR] Scene '%s' already exists\n", _name.c_str() );
		return;
	}

	m_scenes[ _name ] = new cScene( _name );
}

void wv::cSceneManager::switchScene( std::string _scene )
{
	if ( m_next_scene )
	{
		printf( "[ERROR]: Already transitioning to Scene %s\n", m_next_scene->getName().c_str() ); // TODO: change to wv::log or wv::assert
		return;
	}

	if ( m_scenes.count( _scene ) == 0 )
	{
		printf( "[ERROR]: No scene named %s\n", _scene.c_str() ); // TODO: change to wv::log or wv::assert
		return;
	}

	m_next_scene = m_scenes[ _scene ];
}

void wv::cSceneManager::loadScene( cSceneLoader* _scene_loader )
{
	_scene_loader->load( m_active_scene );
}

wv::cVector3f wv::cSceneManager::getDirectionalLightDirection( void )
{
	float pitch = m_directional_light_rotation.x * ( 3.1415f / 180.0f ); // TODO: change to const or wv::math
	float yaw   = m_directional_light_rotation.y * ( 3.1415f / 180.0f );

	return cVector3f{ sin( yaw ),
				   -( sin( pitch ) * cos( yaw ) ),
				   -( cos( pitch ) * cos( yaw ) ) };
}

void wv::cSceneManager::rotateDirectionalLight( cVector2f _rotation )
{
	m_directional_light_rotation += _rotation;
}
