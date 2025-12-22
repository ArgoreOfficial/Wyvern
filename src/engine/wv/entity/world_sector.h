#pragma once

#include <wv/entity/entity.h>

#include <unordered_map>

namespace wv {

typedef uint32_t WorldSectorID;

class WorldSector
{
	friend class World;

public:
	enum class WorldSectorState : uint8_t
	{
		UNLOADED = 0,
		LOADING,
		LOADED,
		INITIALIZED
	};

	WorldSector() = default;
	~WorldSector();

	WorldSectorID getID() const { return m_ID; }

	void load();
	void unload();
	void initialize();
	void shutdown();

	void updateLoading();
	void update( double _deltaTime );

	bool isUnloaded()    const { return m_state == WorldSectorState::UNLOADED; }
	bool isLoading()     const { return m_state == WorldSectorState::LOADING; }
	bool isLoaded()      const { return m_state == WorldSectorState::LOADED; }
	bool isInitialized() const { return m_state == WorldSectorState::INITIALIZED; }

	Entity* findEntity( EntityID _entityID ) const {
		auto it = m_entityMap.find( _entityID );
		if ( it == m_entityMap.end() )
			return nullptr;
		return it->second;
	}

	size_t getNumEntities() const { return m_entities.size(); }
	const std::vector<Entity*>& getEntities() const { return m_entities; }

	void addEntity( Entity* _entity );
	void destroyEntity( EntityID _entityID );

	World* getParentWorld() const { return m_parentWorld; }

protected:
	World* m_parentWorld = nullptr;

	WorldSectorID    m_ID    = wv::Math::randomU32();
	WorldSectorState m_state = WorldSectorState::UNLOADED;

	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_entitiesToLoad;
	std::unordered_map<EntityID, Entity*> m_entityMap;
};

}