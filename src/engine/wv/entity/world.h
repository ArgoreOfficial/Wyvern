#pragma once

#include <wv/reflection/reflection.h>

#include <wv/entity/world_sector.h>

namespace wv {

class WorldSector;

class World : public IReflectedType
{
	WV_REFLECT_TYPE( World ) 
public:

	WorldSector* findSector( WorldSectorID _entityID ) const {
		auto it = m_sectorMap.find( _entityID );
		if ( it == m_sectorMap.end() )
			return nullptr;
		return it->second;
	}

	void addSector( WorldSector* _sector );
	void destroySector( WorldSectorID _sectorID );

	void updateLoading();
	void updateSectors( double _deltaTime );

protected:

	std::vector<WorldSector*> m_sectors;
	std::vector<WorldSector*> m_sectorsToLoad;
	std::unordered_map<WorldSectorID, WorldSector*> m_sectorMap;
	
};

}