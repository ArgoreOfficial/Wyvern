#include "ApplicationState.h"

#include <wv/Debug/Print.h>
#include <wv/Scene/SceneRoot.h>

void wv::cApplicationState::onCreate()
{
	for( auto& scene : m_scenes )
		scene->onCreate();
}

void wv::cApplicationState::onDestroy()
{
	m_pCurrentScene->onUnload();

	for( auto& scene : m_scenes )
	{
		scene->onDestroy();
		delete scene;
	}

	m_scenes.clear();
}

void wv::cApplicationState::update( double _deltaTime )
{
	if( m_pNextScene )
	{
		if( m_pCurrentScene )
			m_pCurrentScene->onUnload();

		m_pNextScene->onLoad();

		Debug::Print( Debug::WV_PRINT_DEBUG, "Switched Scene\n" );
		m_pCurrentScene = m_pNextScene;
		m_pNextScene = nullptr;
	}

	m_pCurrentScene->update( _deltaTime );
}

void wv::cApplicationState::draw( iDeviceContext* _pContext, iGraphicsDevice* _pDevice )
{
	m_pCurrentScene->draw( _pContext, _pDevice );
}

int wv::cApplicationState::addScene( cSceneRoot* _pScene )
{
	// check if scene already exists
	// allow skip?
	std::string name = _pScene->getName();

	for( size_t i = 0; i < m_scenes.size(); i++ )
	{
		if( m_scenes[ i ] == _pScene ||
			m_scenes[ i ]->getName() == name )
		{
			Debug::Print( Debug::WV_PRINT_ERROR, "Scene '%s' already exists\n", name.c_str());
			return i;
		}
	}

	int index = static_cast<int>( m_scenes.size() );
	m_scenes.push_back( _pScene );

	Debug::Print( Debug::WV_PRINT_DEBUG, "Added Scene '%s' (%i)\n", name.c_str(), index );
	return index;
}

void wv::cApplicationState::switchToScene( const std::string& _name )
{
	for( auto& scene : m_scenes )
	{
		if( scene->getName() != _name )
			continue;
		
		Debug::Print( Debug::WV_PRINT_DEBUG, "Switching to scene '%s'\n", _name.c_str() );
		m_pNextScene = scene;
	}

	Debug::Print( Debug::WV_PRINT_ERROR, "Could not find scene '%s'\n", _name.c_str() );
}

void wv::cApplicationState::switchToScene( int _index )
{
	// any error here is the users fault
	m_pNextScene = m_scenes[ _index ];
}
