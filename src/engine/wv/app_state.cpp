#include "app_state.h"

#include <wv/debug/log.h>
#include <wv/Scene/Scene.h>

#include <wv/reflection/reflection.h>
#include <wv/filesystem/file_system.h>
#include <wv/memory/memory.h>

#include <wv/engine.h>
#include <wv/scene/iupdatable.h>
#include <wv/runtime.h>

#include <wv/camera/freeflight_camera.h>
#include <wv/camera/orbit_camera.h>

#include <wv/device/device_context.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::initialize()
{
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
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onConstruct()
{
	for ( auto u : m_updatables )
		u->callOnConstruct();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onDestruct()
{
	for ( auto u : m_updatables )
		u->callOnDestruct();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onEnter()
{
	for ( auto u : m_updatables )
		u->callOnEnter();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onExit()
{
	for ( auto u : m_updatables )
		u->callOnExit();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onUpdate( double _deltaTime )
{
	_addQueued();
	_removeQueued();

	if( m_pNextScene )
	{
		onExit();

		m_pCurrentScene = m_pNextScene;
		m_pNextScene = nullptr;
		Debug::Print( Debug::WV_PRINT_DEBUG, "Switched Scene\n" );
	}
	
	onConstruct();
	onEnter();
	
	m_pCurrentScene->onUpdateTransforms();
	
	for ( auto u : m_updatables )
		u->callOnUpdate( _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onPhysicsUpdate( double _deltaTime )
{
	for ( auto u : m_updatables )
		u->callOnPhysicsUpdate( _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IAppState::onDraw( IDeviceContext* _pContext, IGraphicsDevice* _pDevice )
{
	for ( auto u : m_updatables )
		u->callOnDraw( _pContext, _pDevice );
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

	m_pCurrentScene->destroyAllEntities();

	_addQueued();
	_removeQueued();

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

	m_pCurrentScene = loadScene( Engine::get()->m_pFileSystem, path );
	m_scenes[ index ] = m_pCurrentScene;

	_addQueued();
	_removeQueued();

	onConstruct();
	onEnter();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IEntity* parseSceneObject( const wv::Json& _json )
{
	std::string objTypeName = _json[ "type" ].string_value();

	wv::ParseData parseData; /// TODO: fix
	parseData.json = _json;

	wv::IEntity* obj = nullptr;
	
	wv::RuntimeRegistry* reg = wv::RuntimeRegistry::get();

	if ( !reg->isRuntimeType( objTypeName ) )
	{
		obj = (wv::IEntity*)wv::ReflectionRegistry::parseInstance( objTypeName, parseData );
		obj->_registerUpdatable();
	}
	else
	{
		obj = static_cast<wv::IEntity*>( reg->instantiate( objTypeName ) );
		obj->_registerUpdatable();

		wv::UUID    uuid = _json[ "uuid" ].int_value();
		std::string name = _json[ "name" ].string_value();

		wv::Json tfm = _json[ "transform" ];
		wv::Vector3f pos = wv::jsonToVec3( tfm[ "pos" ].array_items() );
		wv::Vector3f rot = wv::jsonToVec3( tfm[ "rot" ].array_items() );
		wv::Vector3f scl = wv::jsonToVec3( tfm[ "scl" ].array_items() );

		wv::Transformf transform;
		transform.setPosition( pos );
		transform.setRotation( rot );
		transform.setScale( scl );

		obj->_setUUID( uuid );
		obj->setName( name );
		obj->m_transform = transform;
		
		auto keys = _json["data"].object_items();
		for ( auto k : keys )
		{
			if ( obj->hasProperty( k.first ) )
			{
				switch ( k.second.type() )
				{
				case json11::Json::STRING: obj->setPropertyStr( k.first, k.second.string_value() ); break;
				case json11::Json::BOOL:   obj->setPropertyStr( k.first, std::to_string( k.second.bool_value()   ) ); break; // HACK
				case json11::Json::NUMBER: obj->setPropertyStr( k.first, std::to_string( k.second.number_value() ) ); break; // HACK
				}
				//wv::Debug::Print( "has property '%s'\n", k.first.c_str() );
			}
		}

		if( obj->pQuery )
			obj->pQuery->dump();
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

void wv::IAppState::_addQueued()
{
	while ( !m_addedUpdatableQueue.empty() )
	{
		IUpdatable* u = m_addedUpdatableQueue.front();
		m_addedUpdatableQueue.pop();

		m_updatables.push_back( u );
	}
}

void wv::IAppState::_removeQueued()
{
	while ( !m_removedUpdatableQueue.empty() )
	{
		IUpdatable* u = m_removedUpdatableQueue.front();
		m_removedUpdatableQueue.pop();

		std::vector<IUpdatable*>::iterator itr;
		for ( itr = m_updatables.begin(); itr != m_updatables.end(); itr++ )
		{
			if ( *itr != u )
				continue;

			m_updatables.erase( itr );
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IAppState* wv::GetAppState()
{
	Engine* engine = Engine::get();
	if ( !engine )
		return nullptr;

	return engine->m_pAppState;
}
