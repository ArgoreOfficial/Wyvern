#include "ApplicationState.h"

#include <wv/Debug/Print.h>
#include <wv/Scene/Scene.h>

#include <wv/Reflection/Reflection.h>
#include <wv/Memory/FileSystem.h>
#include <wv/Memory/Memory.h>

#include <wv/Engine/Engine.h>
#include <wv/Events/UpdateManager.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::initialize()
{
	m_pUpdateManager = WV_NEW( UpdateManager );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::terminate()
{
	if ( m_pUpdateManager )
		WV_FREE( m_pUpdateManager );

	if ( m_pCurrentScene )
		m_pCurrentScene->destroyAllEntities();

	for ( auto& scene : m_scenes )
		WV_FREE( scene );

	m_scenes.clear();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::onConstruct()
{
	m_pUpdateManager->onConstruct();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::onDestruct()
{
	m_pUpdateManager->onDestruct();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::onEnter()
{
	m_pUpdateManager->onEnter();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::onExit()
{
	m_pUpdateManager->onExit();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::onUpdate( double _deltaTime )
{
	m_pUpdateManager->_updateQueued();

	if( m_pNextScene )
	{
		m_pCurrentScene = m_pNextScene;
		m_pNextScene = nullptr;
		
		onConstruct();
		onEnter();
		
		Debug::Print( Debug::WV_PRINT_DEBUG, "Switched Scene\n" );
	}

	if( m_pCurrentScene )
	{
		// if there are any new updatables that needs construction and entering
		onConstruct();
		onEnter();
	}

	m_pCurrentScene->onUpdate( _deltaTime );
	m_pUpdateManager->onUpdate( _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::onPhysicsUpdate( double _deltaTime )
{
	m_pUpdateManager->onPhysicsUpdate( _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::onDraw( iDeviceContext* _pContext, iLowLevelGraphics* _pDevice )
{
	m_pUpdateManager->onDraw( _pContext, _pDevice );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::reloadScene()
{
	std::string path = m_pCurrentScene->getSourcePath();
	if( path == "" )
	{
		Debug::Print( Debug::WV_PRINT_ERROR, "Cannot reload scene with no source path\n" );
		return;
	}

	onExit();
	onDestruct();

	m_pUpdateManager->_updateQueued();

	m_pCurrentScene->destroyAllEntities();

	int index = -1;
	for( size_t i = 0; i < m_scenes.size(); i++ )
	{
		if( m_scenes[ i ] == m_pCurrentScene )
		{
			index = (int)i;
			break;
		}
	}

	WV_FREE( m_pCurrentScene );
	
	m_pUpdateManager->_updateQueued();

	m_pCurrentScene = loadScene( cEngine::get()->m_pFileSystem, path );
	m_scenes[ index ] = m_pCurrentScene;

	m_pUpdateManager->_updateQueued();

	onConstruct();
	onEnter();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IEntity* parseSceneObject( const wv::Json& _json )
{
	std::string objTypeName = _json[ "type" ].string_value();

	wv::sParseData parseData; /// TODO: fix
	parseData.json = _json;

	wv::IEntity* obj = ( wv::IEntity* )wv::cReflectionRegistry::parseInstance( objTypeName, parseData );

	if( !obj )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Failed to create object of type '%s'\n", objTypeName.c_str() );
		return nullptr;
	}

	for( auto& childJson : _json[ "children" ].array_items() )
	{
		wv::IEntity* childObj = parseSceneObject( childJson );
		if ( childObj != nullptr )
			obj->m_transform.addChild( &childObj->m_transform );
	}

	return obj;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Scene* wv::cApplicationState::loadScene( cFileSystem* _pFileSystem, const std::string& _path )
{
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Loading scene '%s'\n", _path.c_str() );

	std::string src = _pFileSystem->loadString( _path );

	if( src == "" )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Failed to load scene\n" );
		return nullptr;
	}

	std::string err;
	wv::Json root = wv::Json::parse( src, err );

	wv::Scene* scene = WV_NEW( wv::Scene, root[ "name" ].string_value(), _path );
	
	for( auto& objJson : root[ "scene" ].array_items() )
		scene->addChild( parseSceneObject( objJson ) );

	return scene;
}

///////////////////////////////////////////////////////////////////////////////////////

int wv::cApplicationState::addScene( Scene* _pScene )
{
	if( !_pScene )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Cannot add null scene\n" );
		return -1;
	}

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

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

void wv::cApplicationState::switchToScene( int _index )
{
	if ( _index < 0 || _index > m_scenes.size() )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Cannot switch to scene %i. Out of range\n", _index );
		return;
	}

	m_pNextScene = m_scenes[ _index ];
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cApplicationState* wv::getAppState()
{
	cEngine* engine = cEngine::get();
	if ( !engine )
		return nullptr;

	return engine->m_pApplicationState;
}
