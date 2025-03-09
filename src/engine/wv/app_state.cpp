#include "app_state.h"

#include <wv/debug/log.h>
#include <wv/Scene/Scene.h>

#include <wv/reflection/reflection.h>
#include <wv/filesystem/file_system.h>
#include <wv/memory/memory.h>

#include <wv/engine.h>
#include <wv/event/update_manager.h>
#include <wv/runtime.h>

#include <wv/camera/freeflight_camera.h>
#include <wv/camera/orbit_camera.h>

#include <wv/device/device_context.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::initialize()
{
	m_pUpdateManager = WV_NEW( UpdateManager );

	orbitCamera      = WV_NEW( OrbitCamera,      ICamera::kPerspective );
	freeflightCamera = WV_NEW( FreeflightCamera, ICamera::kFocal );

	orbitCamera->onEnter();
	freeflightCamera->onEnter();

	freeflightCamera->getTransform().setPosition( { 0.0f, 0.0f, 20.0f } );
	currentCamera = freeflightCamera;

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::terminate()
{
	currentCamera = nullptr;

	if( orbitCamera )
	{
		WV_FREE( orbitCamera );
		orbitCamera = nullptr;
	}

	if( freeflightCamera )
	{
		WV_FREE( freeflightCamera );
		freeflightCamera = nullptr;
	}
	
	if ( m_pCurrentScene )
		m_pCurrentScene->destroyAllEntities();

	for ( auto& scene : m_scenes )
		WV_FREE( scene );

	m_scenes.clear();

	if ( m_pUpdateManager )
		WV_FREE( m_pUpdateManager );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onConstruct()
{
	m_pUpdateManager->onConstruct();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onDestruct()
{
	m_pUpdateManager->onDestruct();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onEnter()
{
	m_pUpdateManager->onEnter();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onExit()
{
	m_pUpdateManager->onExit();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onUpdate( double _deltaTime )
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

	m_pCurrentScene->onUpdateTransforms();
	m_pUpdateManager->onUpdate( _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onPhysicsUpdate( double _deltaTime )
{
	m_pUpdateManager->onPhysicsUpdate( _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onDraw( IDeviceContext* _pContext, IGraphicsDevice* _pDevice )
{
	m_pUpdateManager->onDraw( _pContext, _pDevice );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::reloadScene()
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

	m_pCurrentScene = loadScene( Engine::get()->m_pFileSystem, path );
	m_scenes[ index ] = m_pCurrentScene;

	m_pUpdateManager->_updateQueued();

	onConstruct();
	onEnter();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IEntity* parseSceneObject( const wv::Json& _json )
{
	std::string objTypeName = _json[ "type" ].string_value();

	wv::ParseData parseData; /// TODO: fix
	parseData.json = _json;

	wv::IEntity* obj = ( wv::IEntity* )wv::ReflectionRegistry::parseInstance( objTypeName, parseData );

	wv::IRuntimeObject* rtObject = wv::RuntimeRegistry::get()->instantiate( objTypeName );
	if ( rtObject )
	{
		auto keys = _json["data"].object_items();
		for ( auto k : keys )
		{
			if ( rtObject->hasProperty( k.first ) )
			{
				switch ( k.second.type() )
				{
				case json11::Json::STRING: rtObject->setPropertyStr( k.first, k.second.string_value() ); break;
				case json11::Json::BOOL:   rtObject->setPropertyStr( k.first, std::to_string( k.second.bool_value()   ) ); break; // HACK
				case json11::Json::NUMBER: rtObject->setPropertyStr( k.first, std::to_string( k.second.number_value() ) ); break; // HACK
				}
				//wv::Debug::Print( "has property '%s'\n", k.first.c_str() );
			}
		}

		rtObject->pQuery->dump();
		WV_FREE( rtObject );
	}

	if( !obj )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Failed to create object of type '%s'\n", objTypeName.c_str() );
		return nullptr;
	}
	
	/*
	for( auto& childJson : _json[ "children" ].array_items() )
	{
		wv::IEntity* childObj = parseSceneObject( childJson );
		if ( childObj != nullptr )
			obj->m_transform.addChild( &childObj->m_transform );
	}
	*/

	return obj;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Scene* wv::IAppState::loadScene( IFileSystem* _pFileSystem, const std::string& _path )
{
	wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Loading scene '%s'\n", _path.c_str() );

	std::string src = "";
	if( _path != "" )
		src = _pFileSystem->loadString( _path );

	if( src == "" )
		return WV_NEW( wv::Scene, "", "" );

	std::string err;
	wv::Json root = wv::Json::parse( src, err );

	wv::Scene* scene = WV_NEW( wv::Scene, root[ "name" ].string_value(), _path );
	
	for( auto& objJson : root[ "scene" ].array_items() )
	{
		scene->addChild( parseSceneObject( objJson ) );
	}

	return scene;
}

///////////////////////////////////////////////////////////////////////////////////////

int wv::IAppState::addScene( Scene* _pScene )
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

void wv::IAppState::switchToScene( const std::string& _name )
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

void wv::IAppState::switchToScene( int _index )
{
	if ( _index < 0 || _index > m_scenes.size() )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Cannot switch to scene %i. Out of range\n", _index );
		return;
	}

	m_pNextScene = m_scenes[ _index ];
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IAppState* wv::GetAppState()
{
	Engine* engine = Engine::get();
	if ( !engine )
		return nullptr;

	return engine->m_pAppState;
}
