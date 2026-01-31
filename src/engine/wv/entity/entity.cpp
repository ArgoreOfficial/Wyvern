#include "entity.h"

#include <wv/entity/entity_component.h>
#include <wv/entity/world.h>

wv::Entity::~Entity()
{
	for ( IEntityComponent* component : m_components )
		WV_FREE( component );

	for ( IEntitySystem* system : m_systems )
		WV_FREE( system );
}

void wv::Entity::load( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state == EntityState::UNLOADED );
	
	for ( auto component : m_components )
		component->load( _ctx );
	
	m_state = EntityState::LOADED;
}

void wv::Entity::unload( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state == EntityState::LOADED );
		
	for ( auto component : m_components )
		component->unload( _ctx );
	
	m_state = EntityState::UNLOADED;
}

void wv::Entity::initialize( World* _world )
{
	WV_ASSERT( m_state == EntityState::LOADED );

	for ( auto system : m_systems )
	{
		system->m_entity = this;
		system->initialize();
	}

	updateLoading();

	m_state = EntityState::INITIALIZED;
}

void wv::Entity::shutdown() 
{
	WV_ASSERT( m_state == EntityState::INITIALIZED );

	for ( auto component : m_components )
	{
		unregisterComponentWithSystems( component );
		m_parentSector->getParentWorld()->queueComponentForUnregistration( this, component );
	}

	for ( auto system : m_systems )
	{
		system->shutdown();
		system->m_entity = nullptr;
	}

	m_state = EntityState::LOADED;
}

void wv::Entity::updateLoading()
{
	if ( m_componentsToRegister.size() > 0 )
	{
		for ( auto component : m_componentsToRegister )
		{
			registerComponentWithSystems( component );
			m_parentSector->getParentWorld()->queueComponentForRegistration( this, component );
		}

		m_componentsToRegister.clear();
	}
}

void wv::Entity::updateSystems( WorldUpdateContext& _ctx )
{
	for ( auto system : m_systems )
		system->update( _ctx );
}

void wv::Entity::createSystem( IEntitySystem* _system )
{
	if ( _system == nullptr )
		return;

	m_systems.push_back( _system );
	
	if ( isInitialized() )
	{
		_system->initialize();

		for ( auto component : m_components )
			_system->registerComponent( component );
	}
}

void wv::Entity::registerComponentWithSystems( IEntityComponent* _component )
{
	if ( _component == nullptr ) 
		return;
	
	for ( auto system : m_systems )
		system->registerComponent( _component );
}

void wv::Entity::unregisterComponentWithSystems( IEntityComponent* _component )
{
	if ( _component == nullptr ) 
		return;

	for ( auto system : m_systems )
		system->unregisterComponent( _component );
}
