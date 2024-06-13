#include "SceneGame.h"

#include <wv/Application/Application.h>
#include <wv/Audio/Audio.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Device/AudioDevice.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/ModelParser.h>
#include <wv/Primitive/Mesh.h>
#include <wv/State/State.h>

#include <wv/Debug/Print.h>
#include <wv/Debug/Draw.h>

#include <ctime>

#include <glad/glad.h>

#include <SDL2/SDL_keycode.h>

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
	m_xwingMesh         = parser.load( "res/meshes/xwing.dae" );
	m_starDestroyerMesh = parser.load( "res/meshes/star-destroyer.dae" );
	
	m_xwingMesh->transform.setScale( { 0.01f } );
	m_starDestroyerMesh->transform.setScale( { 0.01f } );

	m_skybox = parser.load( "res/meshes/skysphere.dae" );

	m_skyMaterial = new wv::Material();
	m_skyMaterial->loadFromFile( "sky_space" );
	if( m_skybox ) 
		m_skybox->primitives[ 0 ]->material = m_skyMaterial;

	m_backgroundMusic = app->audio->loadAudio2D( "Rebel_Attack_Theme.flac" );
	m_backgroundMusic->play( 0.6f, true );

	m_playerShip = new PlayerShip( m_xwingMesh );
	m_playerShip->onCreate();
	m_playerShip->getTransform().setPosition( { 5.0f, 6.5f, -110.0f } );
	m_playerShip->getTransform().setRotation( { 0.0f, 180.0f, 0.0f } );
	m_playerShip->setTargetRotation( m_playerShip->getTransform().rotation );

	m_dummy = new EnemyShip( m_xwingMesh );
	m_dummy->onCreate();
	m_dummy->getTransform().setPosition( { 0.0f, 0.0f, -2.6f } );
	m_dummy->getTransform().setRotation( { 0.0f, 90.0f, 0.0f } );

	// std::srand( std::time( nullptr ) );
	std::srand( 1 );

	for ( int i = 0; i < 4; i++ )
	{
		StarDestroyer* starDestroyer = new StarDestroyer( m_starDestroyerMesh );
		starDestroyer->onCreate();

		float randExitTime = (float)(std::rand() % 300 + 20 ) / 10.0f;
		
		float randX = (float)( std::rand() % 7000 - 2500 ) / 100.0f;
		float randY = (float)( std::rand() % 2000 - 1000 ) / 100.0f;
		float randZ = (float)( std::rand() % 2000 - 1000 ) / 100.0f;
		float randSpeed = (float)( std::rand() & 60 - 30 ) + 190.0f;

		starDestroyer->getTransform().setPosition( { randX, randY, randZ } );
		starDestroyer->setMaxSpeed( randSpeed );
		starDestroyer->exitHyperspace( randExitTime );

		m_starDestroyers.push_back( starDestroyer );
	}

	subscribeInputEvent();
}

void SceneGame::onUnload()
{
	unsubscribeInputEvent();
	
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	delete m_playerShip;
	delete m_dummy;

	for ( int i = 0; i < (int)m_starDestroyers.size(); i++ )
		delete m_starDestroyers[ i ];
	m_starDestroyers.clear();

	device->destroyMesh( &m_xwingMesh );
	device->destroyMesh( &m_skybox );
	device->destroyMesh( &m_starDestroyerMesh );

	m_backgroundMusic->stop();
}

void SceneGame::onCreate()
{
	
}

void SceneGame::onDestroy()
{
	
}

void SceneGame::onInputEvent( wv::InputEvent _event )
{
	if ( _event.buttondown && _event.key == SDLK_ESCAPE )
		wv::Application::get()->m_applicationState->switchToScene( "SceneMenu" );
}

void SceneGame::update( double _deltaTime )
{
	wv::Application* app = wv::Application::get();

	
	wv::Vector3f target{
		std::sin( (float)app->context->getTime() ),
		0.0f,
		std::cos( (float)app->context->getTime() )
	};

	m_dummy->setTarget( m_dummy->getTransform().position + target );
	m_dummy->update( _deltaTime );

	for ( int i = 0; i < m_starDestroyers.size(); i++ )
		m_starDestroyers[ i ]->update(_deltaTime);
	
	m_playerShip->update( _deltaTime );
}

void SceneGame::draw( wv::GraphicsDevice* _device )
{
	/// TODO: remove raw gl calls
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	_device->draw( m_skybox );
	glDepthFunc( GL_LESS );
	glDepthMask( GL_TRUE );

	m_playerShip->draw( _device );
	
	m_dummy->draw( _device );

	for ( int i = 0; i < m_starDestroyers.size(); i++ )
		m_starDestroyers[ i ]->draw( _device );
}
