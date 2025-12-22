#pragma once

#include <wv/reflection/reflection.h>

#include <wv/entity/world_sector.h>
#include <wv/entity/world_system.h>

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

	template<typename Ty>
	void createWorldSystem();
	
	void updateLoading();
	void updateSectors( double _deltaTime );

	void queueComponentForRegistration  ( Entity* _entity, IEntityComponent* _component ) { m_componentsToRegister  .emplace_back( _entity, _component ); }
	void queueComponentForUnregistration( Entity* _entity, IEntityComponent* _component ) { m_componentsToUnregister.emplace_back( _entity, _component ); }

protected:

	void createWorldSystem( IWorldSystem* _system );

	std::vector<IWorldSystem*> m_systems;

	std::vector<WorldSector*> m_sectors;
	std::vector<WorldSector*> m_sectorsToLoad;
	std::unordered_map<WorldSectorID, WorldSector*> m_sectorMap;
	
	// TODO: DeferredTask ?

	std::vector<std::pair<Entity*, IEntityComponent*>> m_componentsToRegister;
	std::vector<std::pair<Entity*, IEntityComponent*>> m_componentsToUnregister;
};

template<typename Ty>
inline void World::createWorldSystem()
{
	static_assert( std::is_base_of<IEntitySystem, Ty>(), "Type must derive from IEntitySystem" );
	IEntitySystem* system = WV_NEW( Ty );
	createWorldSystem( system );
}

}