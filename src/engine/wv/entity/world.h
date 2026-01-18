#pragma once

#include <wv/reflection/reflection.h>

#include <wv/entity/world_sector.h>
#include <wv/entity/world_system.h>

#include <wv/math/vector2.h>
#include <wv/input/input_system.h>

namespace wv {

class WorldSector;
class ViewVolume;
class Viewport;
class InputSystem;

class AssetManager;
class MeshAssetLoader;

struct WorldLoadContext
{
	InputSystem* inputSystem = nullptr;
	AssetManager* worldAssetManager = nullptr;
	MeshAssetLoader* meshAssetLoader = nullptr;
};

struct WorldUpdateContext
{
	Viewport* viewport = nullptr;
	double deltaTime = 0.0;
	
	InputSystem* inputSystem = nullptr;
	std::vector<ActionEvent> actionEventQueue;
};

class World : public IReflectedType
{
	WV_REFLECT_TYPE( World, IReflectedType )
public:
	World() = default;
	~World();

	void shutdown();

	WorldSector* findSector( WorldSectorID _entityID ) const {
		auto it = m_sectorMap.find( _entityID );
		if ( it == m_sectorMap.end() )
			return nullptr;
		return it->second;
	}

	void addSector( WorldSector* _sector );
	void destroySector( WorldSectorID _sectorID );

	template<typename Ty>
	Ty* createWorldSystem();

	template<typename Ty>
	Ty* getWorldSystem() {
		static_assert( std::is_base_of<IWorldSystem, Ty>(), "Type must derive from IWorldSystem" );

		for ( size_t i = 0; i < m_systems.size(); i++ )
		{
			if ( m_systems[ i ]->getTypeUUID() != Ty::getStaticTypeUUID() )
				continue;
			return static_cast<Ty*>( m_systems[ i ] );
		}

		// Does not contain system of type
		return nullptr;
	}

	template<typename Ty>
	void destroyWorldSystem() {
		static_assert( std::is_base_of<IWorldSystem, Ty>(), "Type must derive from IWorldSystem" );

		for ( size_t i = 0; i < m_systems.size(); i++ )
		{
			if ( m_systems[ i ]->getTypeUUID() != Ty::getStaticTypeUUID() )
				continue;

			Ty* system = static_cast<Ty*>( m_systems[ i ] );
			m_systems.erase( m_systems.begin() + i );

			if ( !system )
				continue;

			delete system;
		}
	}

	void updateLoading();
	void updateSectors( double _deltaTime );
	void updateWorldSystems( double _deltaTime );

	void queueComponentForRegistration  ( Entity* _entity, IEntityComponent* _component ) { m_componentsToRegister  .emplace_back( _entity, _component ); }
	void queueComponentForUnregistration( Entity* _entity, IEntityComponent* _component ) { m_componentsToUnregister.emplace_back( _entity, _component ); }

	Viewport* getViewport() const                { return m_viewport; }
	void      setViewport( Viewport* _viewport ) { m_viewport = _viewport; }

protected:

	void createWorldSystem( IWorldSystem* _system );
	void updateRegisterUnregisterComponents();

	std::vector<IWorldSystem*> m_systems;

	std::vector<WorldSector*> m_sectors;
	std::vector<WorldSector*> m_sectorsToLoad;
	std::unordered_map<WorldSectorID, WorldSector*> m_sectorMap;
	
	std::vector<std::pair<Entity*, IEntityComponent*>> m_componentsToRegister;
	std::vector<std::pair<Entity*, IEntityComponent*>> m_componentsToUnregister;

	Viewport* m_viewport = nullptr;

	AssetManager* m_worldAssetManager = nullptr;
};

template<typename Ty>
inline Ty* World::createWorldSystem()
{
	static_assert( std::is_base_of<IWorldSystem, Ty>(), "Type must derive from IEntitySystem" );
	IWorldSystem* system = WV_NEW( Ty );
	createWorldSystem( system );
	return static_cast<Ty*>( system );
}

}