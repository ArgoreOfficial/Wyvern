#include "world.h"

wv::World::~World()
{
	// Destroy world sectors
	for ( WorldSector* sector : m_sectors )
		WV_FREE( sector );
	
	m_sectors.clear();

	// Destroy world systems
	for ( IWorldSystem* system : m_systems )
		WV_FREE( system );

	m_systems.clear();

	if ( activeCamera )
		WV_FREE( activeCamera );
}

void wv::World::shutdown()
{
	// Unload and shutdown

	for ( auto sector : m_sectors )
		sector->shutdown();
	
	for ( auto sector : m_sectors )
		sector->unload();
	
	updateRegisterUnregisterComponents();

	for ( auto system : m_systems ) 
		system->shutdown();

}

void wv::World::addSector( WorldSector* _sector )
{
	if ( findSector( _sector->getID() ) != nullptr )
	{
		WV_LOG_ERROR( "World already contains sector %zu", _sector->getID() );
		return;
	}

	WV_ASSERT_MSG( _sector->m_parentWorld != nullptr, "Sector already has a parent world" );
	
	_sector->m_parentWorld = this;

	m_sectors.push_back( _sector );
	m_sectorsToLoad.push_back( _sector );
	m_sectorMap.emplace( _sector->getID(), _sector );
}

void wv::World::destroySector( WorldSectorID _sectorID )
{
	WorldSector* sector = findSector( _sectorID );
	if ( sector== nullptr )
	{
		WV_LOG_ERROR( "World does not contain sector %zu", _sectorID );
		return;
	}

	if( sector->isInitialized() ) sector->shutdown();
	if( sector->isLoaded() ) sector->unload();

	sector->m_parentWorld = nullptr;

	m_sectorMap.erase( _sectorID );

	for ( size_t i = 0; i < m_sectors.size(); i++ )
	{
		if ( m_sectors[ i ] != sector )
			continue;

		m_sectors.erase( m_sectors.begin() + i );
		break;
	}
}

void wv::World::updateLoading()
{
	for ( auto sector : m_sectorsToLoad )
		sector->load();

	m_sectorsToLoad.clear();

	for ( auto sector : m_sectors )
	{
		sector->updateLoading();
	}

	updateRegisterUnregisterComponents();
}

void wv::World::updateSectors( double _deltaTime )
{
	for ( auto sector : m_sectors )
		sector->update( _deltaTime );
	
	for ( auto system : m_systems )
		system->update( _deltaTime );
	
}

void wv::World::createWorldSystem( IWorldSystem* _system )
{
	if ( _system == nullptr )
		return;

	m_systems.push_back( _system );

	_system->initialize();

	for ( auto sector : m_sectors )
	{
		for ( auto entity : sector->getEntities() )
		{
			
			for ( auto component : entity->getComponents() )
				_system->registerComponent( entity, component );
		}
	}
}

void wv::World::updateRegisterUnregisterComponents()
{
	for ( auto system : m_systems )
	{
		for ( auto component : m_componentsToUnregister )
			system->unregisterComponent( component.first, component.second );

		for ( auto component : m_componentsToRegister )
			system->registerComponent( component.first, component.second );
	}

	m_componentsToUnregister.clear();
	m_componentsToRegister.clear();
}
