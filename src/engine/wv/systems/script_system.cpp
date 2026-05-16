#include "script_system.h"

#include <wv/components/script_component.h>

#include <wv/application.h>
#include <wv/filesystem/file_system.h>

void wv::ScriptSystem::configure( ArchetypeConfig& _config )
{
	_config.addComponentType<ScriptComponent>();
}

void wv::ScriptSystem::onComponentAdded( Archetype* _archetype, size_t _index )
{
	Entity* entity = _archetype->getEntities()[ _index ];
	
	ScriptComponent& script = _archetype->getComponents<ScriptComponent>()[ _index ];

	ScriptInternalData& info = m_scriptInfos[ entity->getID() ];
	info = { };
	
	if ( getWorld()->isEditorState() )
		return;
	
	info.env = sol::environment( m_lua, sol::create, m_lua.globals() );
	
	info.env[ "entity" ] = sol::new_table();
	info.env[ "entity" ][ "getTransform" ] = [ entity ]() -> Transform& { return entity->getTransform(); };

	IFileSystem* fs = getApp()->getFileSystem();

	if ( fs->fileExists( fs->getFullPath( script.path ) ) )
	{
		script.script = fs->loadString( script.path );
		m_lua.script( script.script, info.env );

		if ( m_lua.script( "return update ~= nil", info.env ).get<bool>() == true )
			info.updateEvents = UpdateEventType( info.updateEvents | UpdateEvent_Update );
	}
}

void wv::ScriptSystem::onComponentRemoved( Archetype* _archetype, size_t _index )
{
	UUID id = _archetype->getEntities()[ _index ]->getID();
	if ( m_scriptInfos.contains( id ) )
		m_scriptInfos.erase( id );
}

void wv::ScriptSystem::onInitialize()
{
	m_lua.open_libraries( sol::lib::base, sol::lib::math );

	m_lua.new_usertype<wv::Vector3f>( 
		"Vector3f",
		sol::constructors<wv::Vector3f( float, float, float )>(),

		"length",
		&Vector3f::length,

		"x", &Vector3f::x,
		"y", &Vector3f::y,
		"z", &Vector3f::z
	);

	m_lua.new_usertype<Transform>(
		"Transform",
		sol::constructors<Transform>(),

		"position",
		&Transform::position
	);

	m_lua[ "game" ] = sol::new_table();
	m_lua[ "game" ][ "time" ]      = []() -> float { return getApp()->getApplicationTime(); };
	m_lua[ "game" ][ "deltaTime" ] = []() -> float { return getApp()->getDeltaTime(); };
}

void wv::ScriptSystem::onPreUpdate()
{
}

void wv::ScriptSystem::onUpdate()
{
	for ( Archetype* arch : getArchetypes() )
	{
		auto& scripts = arch->getComponents<ScriptComponent>();
		auto& entities = arch->getEntities();

		for ( size_t i = 0; i < arch->getNumEntities(); i++ )
		{
			ScriptComponent& script = scripts[ i ];
			UUID id = entities[ i ]->getID();

			if ( m_scriptInfos.contains( id ) )
			{
				ScriptInternalData& info = m_scriptInfos.at( id );
				if ( info.updateEvents & UpdateEvent_Update )
					m_lua.script( "update()", info.env );
			}
		}
	}
}

void wv::ScriptSystem::onPhysicsUpdate()
{
}
