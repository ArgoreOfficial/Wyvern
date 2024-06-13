#include "StateGame.h"

#include <wv/Scene/Scene.h>
#include <App/Scenes/SceneGame.h>
#include <App/Scenes/SceneMenu.h>
#include <App/Scenes/SceneTexture.h>

StateGame::StateGame()
{
	
}

StateGame::~StateGame()
{

}

void StateGame::onLoad()
{
	// State::onLoad();

	switchToScene( "SceneMenu" );
}

void StateGame::onUnload()
{
	// State::onUnload();
}

void StateGame::onCreate()
{
	addScene<SceneMenu>( "SceneMenu" );
	addScene<SceneGame>( "SceneGame" );
	addScene<SceneTexture>( "SceneTexture" );

	State::onCreate();
}

void StateGame::onDestroy()
{
	State::onDestroy();
}

void StateGame::update( double _deltaTime )
{
	if ( m_currentScene )
		m_currentScene->update( _deltaTime );

	State::update( _deltaTime );
}

void StateGame::draw( wv::GraphicsDevice* _device )
{
	if( m_currentScene )
		m_currentScene->draw( _device );
}
