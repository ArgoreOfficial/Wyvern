#pragma once

#include <wv/math/transform.h>
#include <wv/entity/entity_system.h>

#include <wv/reflection/reflection.h>
#include <wv/memory/memory.h>
#include <wv/types.h>

namespace wv {

class World;

typedef uint32_t EntityID;

class Entity final : wv::IReflectedType
{
	friend class WorldSector;

	WV_REFLECT_TYPE( Entity )
public:
	
	/*
	Unloaded     -> created but no data present
	Loaded       -> data has been loaded in, but entity does not "exist" yet
	Initiailized -> all components, systems, and entity data has been intiailized and the entity exists in world now
	*/

	enum class EntityState : uint8_t
	{
		UNLOADED = 0,
		LOADED,
		INITIALIZED
	};

	Entity() = default;
	~Entity();

	void load();
	void unload();

	void initialize( World* _world );
	void shutdown();

	EntityID getID() const { return m_ID; }
	WorldSector* getParentSector() const { return m_parentSector; }

	Transformf& getTransform() { return m_transform; }

	bool isUnloaded()    const { return m_state == EntityState::UNLOADED; }
	bool isLoaded()      const { return m_state == EntityState::LOADED; }
	bool isInitialized() const { return m_state == EntityState::INITIALIZED; }

	const std::vector<IEntityComponent*>& getComponents() const { return m_components; }
	void addComponent( IEntityComponent* _component ) {
		if ( _component == nullptr )
			return;

		m_components.push_back( _component );
		m_componentsToRegister.push_back( _component );
	}

	template<typename Ty>
	void createSystem();

	template<typename Ty>
	Ty* getSystem() {
		static_assert( std::is_base_of<IEntitySystem, Ty>(), "Type must derive from IEntitySystem" );

		for ( size_t i = 0; i < m_systems.size(); i++ )
		{
			if ( m_systems[ i ]->getTypeUUID() != Ty::typeUUID() )
				continue;
			return static_cast<Ty*>( m_systems[ i ] );
		}

		// Does not contain system of type
		return nullptr;
	}

	template<typename Ty>
	void destroySystem() {
		static_assert( std::is_base_of<IEntitySystem, Ty>(), "Type must derive from IEntitySystem" );

		for ( size_t i = 0; i < m_systems.size(); i++ )
		{
			if ( m_systems[ i ]->getTypeUUID() != Ty::typeUUID() )
				continue;

			Ty* system = static_cast<Ty*>( m_systems[ i ] );
			m_systems.erase( m_systems.begin() + i );

			if ( !system )
				continue;

			delete system;
		}
	}

	void updateLoading();
	void updateSystems( double _deltaTime );

private:
	// The sector that this entity originates from. 
	// This may be different from the one it's currently in
	WorldSector* m_parentSector = nullptr;

	void createSystem( IEntitySystem* _system );

	void registerComponentWithSystems  ( IEntityComponent* _component );
	void unregisterComponentWithSystems( IEntityComponent* _component );

	EntityID    m_ID    = wv::Math::randomU32();
	EntityState m_state = EntityState::UNLOADED;

	Transformf m_transform;

	std::vector<IEntitySystem*>    m_systems;
	std::vector<IEntityComponent*> m_components;
	std::vector<IEntityComponent*> m_componentsToRegister;
};

template<typename Ty>
inline void Entity::createSystem()
{
	static_assert( std::is_base_of<IEntitySystem, Ty>(), "Type must derive from IEntitySystem" );
	IEntitySystem* system = WV_NEW( Ty );
	createSystem( system );
}

}