#pragma once

#include <wv/entity/entity.h>

#include <unordered_map>
#include <queue>

namespace wv {

typedef uint32_t WorldSectorID;

class MeshManager;
class MaterialManager;

class WorldSector
{
	friend class World;

public:
	WorldSector();
	~WorldSector();

	WorldSectorID getID() const { return m_ID; }

	void unload( WorldLoadContext& _ctx );

	void updateLoading( WorldLoadContext& _ctx );
	void update( WorldUpdateContext& _ctx );

	bool isLoading() const {
		for ( auto ent : m_entities )
			if ( ent->isLoading() )
				return true;

		return false;
	}

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

	WorldSectorID m_ID = wv::Math::randomU32();
	
	std::vector<Entity*> m_entities{};
	std::unordered_map<EntityID, Entity*> m_entityMap{};

	std::queue<Entity*> m_entityAddQueue{};
	std::queue<Entity*> m_entityDestroyQueue{};
};

}