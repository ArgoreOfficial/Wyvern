#include "ApplicationState.h"

#include <wv/Debug/Print.h>
#include <wv/Scene/SceneRoot.h>

#include <wv/Reflection/Reflection.h>
#include <wv/Memory/MemoryDevice.h>

#include <wv/Engine/Engine.h>

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

void wv::cApplicationState::reloadScene()
{
	std::string path = m_pCurrentScene->getSourcePath();
	if( path == "" )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot reload scene with no source path\n" );
		return;
	}

	m_pCurrentScene->onDestroy();
	m_pCurrentScene->onUnload();
	
	int index = -1;
	for( size_t i = 0; i < m_scenes.size(); i++ )
	{
		if( m_scenes[ i ] == m_pCurrentScene )
		{
			index = (int)i;
			break;
		}
	}

	delete m_pCurrentScene;

	m_pCurrentScene = loadScene( cEngine::get()->m_pFileSystem, path );
	m_scenes[ index ] = m_pCurrentScene;

	m_pCurrentScene->onCreate();
	m_pCurrentScene->onLoad();
}

wv::iSceneObject* parseSceneObject( fkyaml::node& _yaml )
{
	std::string objTypeName = _yaml[ "type" ].get_value<std::string>();
	wv::iSceneObject* obj = ( wv::iSceneObject* )wv::cReflectionRegistry::createInstanceYaml( objTypeName, _yaml );

	if( !obj )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Failed to create object of type '%s'\n", objTypeName.c_str() );
		return nullptr;
	}

	for( auto& childJson : _yaml[ "children" ] )
	{
		wv::iSceneObject* childObj = parseSceneObject( childJson );
		if( childObj != nullptr )
			obj->addChild( childObj );
	}

	return obj;
}

wv::cSceneRoot* wv::cApplicationState::loadScene( cFileSystem* _pFileSystem, const std::string& _path )
{
	std::string src = _pFileSystem->loadString( _path );
	fkyaml::node root = fkyaml::node::deserialize( src );

	wv::cSceneRoot* scene = new wv::cSceneRoot( root[ "name" ].get_value<std::string>(), _path);
	
	for( auto& objJson : root[ "scene" ] )
		scene->addChild( parseSceneObject( objJson ) );

	return scene;
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
