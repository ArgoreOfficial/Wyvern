#include "SceneGame.h"

#include <wv/Application/Application.h>
#include <wv/Audio/Audio.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Debug/Print.h>
#include <wv/Device/AudioDevice.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/ModelParser.h>
#include <wv/Primitive/Mesh.h>

#include <glad/glad.h>

SceneGame::SceneGame()
{

}

SceneGame::~SceneGame()
{

}

void SceneGame::onLoad()
{
	wv::Application* app = wv::Application::get();
	
	wv::assimp::Parser parser;
	m_xwing = parser.load( "res/meshes/xwing.dae" );
	m_starDestroyer = parser.load( "res/meshes/star-destroyer.dae" );

	m_skybox = parser.load( "res/meshes/skysphere.dae" );

	wv::Material* skyMaterial = new wv::Material(); // memory leak
	skyMaterial->loadFromFile( "sky_space" );
	if( m_skybox ) m_skybox->primitives[ 0 ]->material = skyMaterial;

	m_startupSound = app->audio->loadAudio2D( "psx.flac" );
	
	m_playerShip = new PlayerShip( m_xwing );
	m_playerShip->onCreate();

	m_dummy = new EnemyShip( m_xwing );
	m_dummy->onCreate();

	m_dummy->getTransform().setPosition( { 0.0f, 0.0f, -260.0f } );
	m_dummy->getTransform().setRotation( { 0.0f, 90.0f, 0.0f } );
}

void SceneGame::onUnload()
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	device->destroyMesh( &m_xwing );
	device->destroyMesh( &m_skybox );
}

void SceneGame::onCreate()
{
	
}

void SceneGame::onDestroy()
{
	
}

void SceneGame::update( double _deltaTime )
{
	wv::Application* app = wv::Application::get();
	
	if ( !m_hasPlayedStartup && app->audio->isUnlocked() && !m_startupSound->isPlaying() )
	{
		m_startupSound->play();
		m_hasPlayedStartup = true;
	}

	m_playerShip->update( _deltaTime );
	wv::Vector3f target{
		std::sin( (float)app->context->getTime() ),
		0.0f,
		std::cos( (float)app->context->getTime() )
	};

	m_dummy->setTarget( m_dummy->getTransform().position + target );
	m_dummy->update( _deltaTime );
}

void SceneGame::draw()
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	/// TODO: remove raw gl calls
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	device->draw( m_skybox );
	glDepthFunc( GL_LESS );
	glDepthMask( GL_TRUE );

	m_playerShip->draw( device );
	m_dummy->draw( device );
	
	m_xwing->transform = {};
	device->draw( m_xwing );
	device->draw( m_starDestroyer );
}
