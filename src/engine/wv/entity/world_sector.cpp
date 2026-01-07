#include "world_sector.h"

wv::WorldSector::~WorldSector()
{
	for ( Entity* entity : m_entities )
		WV_FREE( entity );

	m_entities.clear();
}

void wv::WorldSector::load()
{
	WV_ASSERT( m_state != WorldSectorState::UNLOADED );

	m_state = WorldSectorState::LOADING;

	int numFailedLoads = 0;
	for ( auto entity : m_entitiesToLoad )
	{
		entity->load();
		if ( !entity->isLoaded() )
			numFailedLoads++;
	}

	m_entitiesToLoad.clear();

	// if ( numFailedLoads == 0 )
	m_state = WorldSectorState::LOADED;
}

void wv::WorldSector::unload()
{
	WV_ASSERT( m_state != WorldSectorState::LOADED );

	for ( auto entity : m_entities )
	{
		if( entity->isLoaded() )
			entity->unload();
	}

	m_state = WorldSectorState::UNLOADED;
}

void wv::WorldSector::initialize()
{
	WV_ASSERT( m_state != WorldSectorState::LOADED );

	for ( auto entity : m_entities )
	{
		if ( entity->isLoaded() )
			entity->initialize( m_parentWorld );
	}

	for ( auto entity : m_entities )
		entity->updateLoading();

	m_state = WorldSectorState::INITIALIZED;
}

void wv::WorldSector::shutdown()
{
	WV_ASSERT( m_state != WorldSectorState::INITIALIZED );

	for ( auto entity : m_entities )
	{
		if ( entity->isInitialized() )
			entity->shutdown();
	}

	m_state = WorldSectorState::LOADED;
}

void wv::WorldSector::updateLoading()
{
	if ( isLoaded() ) // remove?
		initialize();

	for ( auto entity : m_entities )
		entity->updateLoading();
}

void wv::WorldSector::update( WorldUpdateContext& _ctx )
{
	for ( auto entity : m_entities )
	{
		if ( entity->isInitialized() )
			entity->updateSystems( _ctx );
	}
}

void wv::WorldSector::addEntity( Entity* _entity )
{
	if ( findEntity( _entity->getID() ) != nullptr )
	{
		WV_LOG_ERROR( "World sector %zu already contains entity %zu", m_ID, _entity->getID() );
		return;
	}

	_entity->m_parentSector = this;

	m_entities.push_back( _entity );
	m_entitiesToLoad.push_back( _entity );
	m_entityMap.emplace( _entity->getID(), _entity );
}

void wv::WorldSector::destroyEntity( EntityID _entityID )
{
	Entity* entity = findEntity( _entityID );
	if ( entity == nullptr )
	{
		WV_LOG_ERROR( "World sector %zu does not contain entity %zu", m_ID, _entityID );
		return;
	}

	entity->m_parentSector = nullptr;

	m_entityMap.erase( _entityID );
	
	for ( size_t i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] != entity )
			continue;

		m_entities.erase( m_entities.begin() + i );
		break;
	}
}
