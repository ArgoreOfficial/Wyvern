#pragma once

#include <wv/entity/entity.h>

#include <wv/types.h>

#include <unordered_map>
#include <queue>

namespace wv {

class MeshManager;
class MaterialManager;

class WorldSector
{
	friend class World;

public:
	WorldSector();
	~WorldSector();

	UUID getID() const { return m_ID; }

	void unload( WorldLoadContext& _ctx );

	void updateLoading( WorldLoadContext& _ctx );
	void update( WorldUpdateContext& _ctx );

	bool isLoading() const {
		for ( auto ent : m_entities )
			if ( ent->isLoading() )
				return true;

		return false;
	}

	Entity* findEntity( UUID _UUID ) const {
		auto it = m_entityMap.find( _UUID );
		if ( it == m_entityMap.end() )
			return nullptr;
		return it->second;
	}

	size_t getNumEntities() const { return m_entities.size(); }
	const std::vector<Entity*>& getEntities() const { return m_entities; }

	void addEntity( Entity* _entity );
	void destroyEntity( UUID _UUID );

	World* getParentWorld() const { return m_parentWorld; }

protected:
	World* m_parentWorld = nullptr;

	UUID m_ID = wv::Math::randomU32();
	
	std::vector<Entity*> m_entities{};
	std::unordered_map<UUID, Entity*> m_entityMap{};

	std::queue<Entity*> m_entityAddQueue{};
	std::queue<Entity*> m_entityDestroyQueue{};
};

}