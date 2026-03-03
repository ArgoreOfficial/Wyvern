#include "world_sector.h"

#include <wv/entity/world.h>

#include <wv/application.h>
#include <wv/thread/task_system.h>

wv::WorldSector::WorldSector()
{
	
}

wv::WorldSector::~WorldSector()
{
	for ( Entity* entity : m_entities )
		WV_FREE( entity );

	m_entities.clear();
}

void wv::WorldSector::unload( WorldLoadContext& _ctx )
{
	for ( Entity* entity : m_entities )
	{
		entity->unload();
		WV_FREE( entity );
	}
	
	m_entities.clear();
	m_entityMap.clear();
}

void wv::WorldSector::updateLoading( WorldLoadContext& _ctx )
{
	// Add any queued entities from previous frame
	while ( !m_entityAddQueue.empty() )
	{
		Entity* entity = m_entityAddQueue.front(); 
		m_entityAddQueue.pop();

		m_entities.push_back( entity );
		m_entityMap.emplace( entity->getID(), entity );
	}

	// Destroy any queued entities from previous frame
	while ( !m_entityDestroyQueue.empty() )
	{
		Entity* entity = m_entityDestroyQueue.front(); 
		m_entityDestroyQueue.pop();

		entity->unload();
		WV_FREE( entity );
	}

	for ( auto entity : m_entities )
		entity->updateLoading( _ctx );
	
}

void wv::WorldSector::update( WorldUpdateContext& _ctx )
{
	for ( auto entity : m_entities )
	{
		entity->updateSystems( _ctx );
		entity->m_transform.update( nullptr );
	}

}

void wv::WorldSector::onDebugRender()
{
	for ( auto entity : m_entities )
		entity->onDebugRender();
}

void wv::WorldSector::addEntity( Entity* _entity )
{
	if ( findEntity( _entity->getID() ) != nullptr )
	{
		WV_LOG_ERROR( "World sector %zu already contains entity %zu", m_ID, _entity->getID() );
		return;
	}

	_entity->m_parentSector = this;

	m_entityAddQueue.push( _entity );
}

void wv::WorldSector::destroyEntity( UUID _UUID )
{
	Entity* entity = findEntity( _UUID );
	if ( entity == nullptr )
	{
		WV_LOG_ERROR( "World sector %zu does not contain entity %zu", m_ID, _UUID );
		return;
	}

	for ( auto it = m_entities.begin(); it != m_entities.end(); it++ )
	{
		if ( *it != entity ) continue;
		m_entities.erase( it );
		break;
	}

	m_entityMap.erase( _UUID );
	m_entityDestroyQueue.push( entity );
}
