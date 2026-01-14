#include "world_sector.h"

#include <wv/entity/world.h>

#include <wv/filesystem/asset_manager.h>
#include <wv/filesystem/loaders/mesh_asset_loader.h>

#include <wv/application.h>

wv::WorldSector::WorldSector()
{
	IFileSystem* filesystem = Application::getSingleton()->getFileSystem();
	m_assetManager    = WV_NEW( AssetManager );
	m_meshAssetLoader = WV_NEW( MeshAssetLoader, filesystem, m_assetManager );
}

wv::WorldSector::~WorldSector()
{
	for ( Entity* entity : m_entities )
		WV_FREE( entity );

	m_entities.clear();

	// free loaders
	WV_FREE( m_meshAssetLoader );

	// free asset manager
	WV_FREE( m_assetManager );
}

void wv::WorldSector::load( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state != WorldSectorState::UNLOADED );

	m_state = WorldSectorState::LOADING;

	_ctx.meshAssetLoader = m_meshAssetLoader;

	int numFailedLoads = 0;
	for ( auto entity : m_entitiesToLoad )
	{
		entity->load( _ctx );
		if ( !entity->isLoaded() )
			numFailedLoads++;
	}

	m_entitiesToLoad.clear();

	// if ( numFailedLoads == 0 )
	m_state = WorldSectorState::LOADED;
}

void wv::WorldSector::unload( WorldLoadContext& _ctx )
{
	WV_ASSERT( m_state != WorldSectorState::LOADED );

	_ctx.meshAssetLoader = m_meshAssetLoader;

	for ( auto entity : m_entities )
	{
		if( entity->isLoaded() )
			entity->unload( _ctx );
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

void wv::WorldSector::updateLoading( WorldLoadContext& _ctx )
{
	if ( isLoaded() ) // remove?
		initialize();

	_ctx.meshAssetLoader = m_meshAssetLoader;

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
