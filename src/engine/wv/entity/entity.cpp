#include "entity.h"

void wv::Entity::initialize() 
{
	if ( m_state != EntityState::LOADED )
		return; // assert

	for ( auto system : m_systems )
		system->initialize();

	for ( auto component : m_components )
		registerComponentWithSystems( component );

	m_state = EntityState::INITIALIZED;
}

void wv::Entity::shutdown() 
{
	if ( m_state != EntityState::INITIALIZED ) 
		return; // assert

	for ( auto component : m_components )
		unregisterComponentWithSystems( component );

	for ( auto system : m_systems )
		system->shutdown();

	m_state = EntityState::LOADED;
}

void wv::Entity::updateSystems( double _deltaTime )
{
	for ( auto system : m_systems )
		system->update( _deltaTime );

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
	if ( _component == nullptr ) return;
	
	for ( auto system : m_systems )
		system->registerComponent( _component );
}

void wv::Entity::unregisterComponentWithSystems( IEntityComponent* _component )
{
	if ( _component == nullptr ) return;

	for ( auto system : m_systems )
		system->unregisterComponent( _component );
}
