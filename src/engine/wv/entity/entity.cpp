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

void wv::Entity::initialize()
{

}

void wv::Entity::unload() 
{
	WorldLoadContext ctx = m_parentSector->getParentWorld()->getLoadContext();

	for ( auto component : m_components )
	{
		component->shutdown();
		component->unload( ctx );

		unregisterComponent( component );
		m_parentSector->getParentWorld()->unregisterComponent( this, component );

		WV_FREE( component );
	}

	for ( auto system : m_systems )
	{
		system->shutdown();

		WV_FREE( system );
	}

	m_components.clear();
	m_systems.clear();
}

bool wv::Entity::isLoading() const
{
	for ( auto comp : m_components )
		if ( comp->isLoading() )
			return true;

	return false;
}

void wv::Entity::updateLoading( WorldLoadContext& _ctx )
{
	for ( auto comp : m_components )
	{
		if( comp->isUnloaded() || comp->isLoading() )
			comp->load( _ctx );

		if ( comp->isLoaded() )
		{
			comp->initialize();

			registerComponent( comp );
			m_parentSector->getParentWorld()->registerComponent( this, comp );
		}
	}
}

void wv::Entity::updateSystems( WorldUpdateContext& _ctx )
{
	for ( auto system : m_systems )
		system->update( _ctx );
}

void wv::Entity::onDebugRender()
{
	if ( !shouldShowDebug )
		return;

	for ( auto system : m_systems )
		system->onDebugRender();
}

void wv::Entity::createSystem( IEntitySystem* _system )
{
	if ( _system == nullptr )
		return;

	_system->m_entity = this;
	_system->initialize();

	m_systems.push_back( _system );
}

void wv::Entity::registerComponent( IEntityComponent* _component )
{
	if ( _component == nullptr ) 
		return;
	
	for ( auto system : m_systems )
		system->registerComponent( _component );
}

void wv::Entity::unregisterComponent( IEntityComponent* _component )
{
	if ( _component == nullptr ) 
		return;

	for ( auto system : m_systems )
		system->unregisterComponent( _component );
}
