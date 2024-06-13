#include "StateGame.h"

#include <wv/Scene/Scene.h>
#include <App/Scenes/SceneGame.h>
#include <App/Scenes/SceneMenu.h>

StateGame::StateGame()
{
	m_currentScene = new SceneMenu();
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

void StateGame::draw()
{
	if( m_currentScene )
		m_currentScene->draw();
}
