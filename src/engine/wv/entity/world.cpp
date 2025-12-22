#include "world.h"

void wv::World::addSector( WorldSector* _sector )
{
	if ( findSector( _sector->getID() ) != nullptr )
	{
		WV_LOG_ERROR( "World already contains sector %zu", _sector->getID() );
		return;
	}

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
		if( sector->isLoaded() )
			sector->initialize();
	}
}

void wv::World::updateSectors( double _deltaTime )
{
	for ( auto sector : m_sectors )
	{
		sector->update( _deltaTime );
	}
}
