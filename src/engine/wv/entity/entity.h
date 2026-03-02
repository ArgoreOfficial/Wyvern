#pragma once

#include <wv/math/transform.h>
#include <wv/entity/entity_system.h>

#include <wv/reflection/reflection.h>
#include <wv/memory/memory.h>
#include <wv/types.h>

namespace wv {

class World;

struct WorldLoadContext;
struct WorldUpdateContext;

class Entity final : wv::IReflectedType
{
	friend class WorldSector;

	WV_REFLECT_TYPE( Entity, IReflectedType )
public:
	Entity( const std::string& _debugName = "" ) : 
		m_ID{ wv::Math::randomU32() },
		m_debugName{ _debugName }
	{ }

	Entity( UUID _id, const std::string& _debugName = "" ) : 
		m_ID{ _id },
		m_debugName{ _debugName }
	{ }

	~Entity();

	void initialize();
	void unload();

	UUID getID() const { return m_ID; }
	std::string getName() { return m_debugName; }
	WorldSector* getParentSector() const { return m_parentSector; }

	Transformf& getTransform() { return m_transform; }

	bool isLoading() const;

	const std::vector<IEntityComponent*>& getComponents() const { return m_components; }
	void addComponent( IEntityComponent* _component ) {
		if ( _component == nullptr )
			return;

		m_components.push_back( _component );
	}

	template<typename Ty>
	Ty* createComponent();

	template<typename Ty>
	Ty* createSystem();

	template<typename Ty>
	Ty* getSystem();

	template<typename Ty>
	void destroySystem();

	void updateLoading( WorldLoadContext& _ctx );
	void updateSystems( WorldUpdateContext& _ctx );

	void onDebugRender();

	bool shouldShowDebug = false;

private:
	// The sector that this entity originates from. 
	// This may be different from the one it's currently in
	WorldSector* m_parentSector = nullptr;

	void createSystem( IEntitySystem* _system );

	void registerComponent  ( IEntityComponent* _component );
	void unregisterComponent( IEntityComponent* _component );

	UUID m_ID = 0;
	std::string m_debugName = "";

	Transformf m_transform;

	std::vector<IEntitySystem*>    m_systems;
	std::vector<IEntityComponent*> m_components;

private:
	WV_REFLECT_MEMBER( m_ID, "UUID" )
	WV_REFLECT_MEMBER( m_debugName, "DebugName" )
	WV_REFLECT_MEMBER( m_transform, "Transform" )
};

template<typename Ty>
inline Ty* Entity::createComponent()
{
	static_assert( std::is_base_of<IEntityComponent, Ty>(), "Type must derive from IEntityComponent" );
	IEntityComponent* component = WV_NEW_NAMED( Ty, Ty::getStaticTypeName() );
	addComponent( component );
	return static_cast<Ty*>( component );
}

template<typename Ty>
inline Ty* Entity::createSystem()
{
	static_assert( std::is_base_of<IEntitySystem, Ty>(), "Type must derive from IEntitySystem" );
	IEntitySystem* system = WV_NEW_NAMED( Ty, Ty::getStaticTypeName() );
	createSystem( system );
	return static_cast<Ty*>( system );
}

template<typename Ty>
inline Ty* Entity::getSystem()
{
	static_assert( std::is_base_of<IEntitySystem, Ty>(), "Type must derive from IEntitySystem" );

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
inline void Entity::destroySystem()
{
	static_assert( std::is_base_of<IEntitySystem, Ty>(), "Type must derive from IEntitySystem" );

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

}