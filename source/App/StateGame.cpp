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
	if ( m_currentScene )
		m_currentScene->onLoad();
}

void StateGame::onUnload()
{
	if ( m_currentScene )
		m_currentScene->onUnload();
}

void StateGame::onCreate()
{
	if ( m_currentScene )
		m_currentScene->onCreate();
}

void StateGame::onDestroy()
{
	if ( m_currentScene )
		m_currentScene->onDestroy();
}

void StateGame::update( double _deltaTime )
{
	if ( m_currentScene )
		m_currentScene->update( _deltaTime );
}

void StateGame::draw()
{
	if( m_currentScene )
		m_currentScene->draw();
}
