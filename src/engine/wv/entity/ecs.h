#pragma once

#include <wv/entity/entity.h>
#include <wv/memory/memory.h>
#include <wv/slot_map.h>
#include <wv/updatable.h>

#include <bitset>
#include <vector>
#include <unordered_map>

namespace wv {

class Entity;
class Archetype;
class ECSEngine;
class ISystem;

struct ArchetypeConfig;

static std::bitset<256> bitmaskFromComponentTypeIndices( const std::vector<int>& _componentTypeIndices )
{
	std::bitset<256> bitmask{};
	for ( int i : _componentTypeIndices )
	{
		if ( i < 0 )
			continue;
		bitmask[ i ] = true;
	}
	return bitmask;
}

struct IComponentContainer
{
	/**
	 * @brief move component between `_from` to this container
	 * @return new index of component
	 */
	virtual size_t moveComponent( IComponentContainer* _from, size_t _index ) = 0;
	virtual void eraseComponent( size_t _index ) = 0;
	virtual size_t getRealIndex( size_t _indirectIndex ) const = 0;

};

template<typename Ty>
struct ComponentContainer : public IComponentContainer
{
	wv::SlotMap<Ty> components;

	virtual size_t moveComponent( IComponentContainer* _from, size_t _index ) override {
		wv::SlotMap<Ty>& fromComponents = ( ( ComponentContainer<Ty>* )_from )->components;
		size_t newIndex = components.push( fromComponents[ _index ] );
		fromComponents.erase( _index );
		return newIndex;
	}

	virtual void eraseComponent( size_t _index ) override {
		components.erase( _index );
	}

	virtual size_t getRealIndex( size_t _indirectIndex ) const override {
		return components.dataIndices[ _indirectIndex ];
	}
};

class ECSEngine
{
public:
	template<typename Ty>
	struct ComponentTypeDef { static inline int index = -1; };
	
	template<typename Ty>
	struct SystemTypeDef { static inline int index = -1; };

	~ECSEngine();

	template<typename Ty>
	int registerComponentType();

	template<typename Ty>
	int registerSystemType();

	template<typename Ty>
	Ty* addSystem();

	template<typename Ty>
	Ty* getSystem() {
		static_assert( std::is_base_of<ISystem, Ty>(), "Type must derive from wv::ISystem" );

		int index = SystemTypeDef<Ty>::index;
		if ( !m_systemIndexMap.contains( index ) )
			return nullptr;

		return (Ty*)m_systemIndexMap.at( index );
	}

	template<typename Ty>
	void eraseSystem() {
		static_assert( std::is_base_of<ISystem, Ty>(), "Type must derive from wv::ISystem" );

		int index = SystemTypeDef<Ty>::index;
		if ( !m_systemIndexMap.contains( index ) )
			return;

		m_systemIndexMap.erase( index );
	}

	Archetype* registerArchetype( ArchetypeConfig& _config );
	Archetype* registerArchetype( std::bitset<256> _bitmask );
	Archetype* getExactArchetype( std::bitset<256> _bitmask );

	void updateSystemsArchetypes();
	
	template<typename Ty>
	void addComponent( Entity* _entity, const Ty& _component );

	template<typename Ty>
	void removeComponent( Entity* _entity );

	void removeAllComponents( Entity* _entity );

	int numComponentTypes = 0;
	int numSystemTypes = 0;

	std::vector<Archetype*> m_archetypes;
	std::unordered_map<int, ISystem*> m_systemIndexMap;

	std::unordered_map<int, std::function<IComponentContainer* ( )>> m_componentVectorFuns;
};

struct ArchetypeConfig
{
	std::vector<int> componentTypeIndices{};

	template<typename Ty>
	void addComponentType() { addComponentType( ECSEngine::ComponentTypeDef<Ty>::index ); }
	void addComponentType( int _typeIndex );

	std::bitset<256> getBitmask() {
		return bitmaskFromComponentTypeIndices( componentTypeIndices );
	}
};

class Archetype
{
	friend class ECSEngine;
	friend class World;

public:
	~Archetype() {
		for ( auto pair : m_containers )
			WV_FREE( pair.second );

		m_containers.clear();
		m_entities.clear();
	}

	template<typename Ty>
	std::vector<Ty>& getComponents() {
		int componentTypeIndex = ECSEngine::ComponentTypeDef<Ty>::index;
		WV_ASSERT_MSG( componentTypeIndex >= 0, "Component type not registered" );

		ComponentContainer<Ty>* vec = reinterpret_cast<ComponentContainer<Ty>*>( m_containers.at( componentTypeIndex ) );
		return vec->components.data;
	}

	template<typename Ty>
	wv::SlotMap<Ty>& getComponentContainer() {
		int componentTypeIndex = ECSEngine::ComponentTypeDef<Ty>::index;
		WV_ASSERT_MSG( componentTypeIndex >= 0, "Component type not registered" );

		ComponentContainer<Ty>* vec = reinterpret_cast<ComponentContainer<Ty>*>( m_containers.at( componentTypeIndex ) );
		return vec->components;
	}

	size_t getNumEntities() const { return m_entities.size(); }
	
	std::vector<Entity*>& getEntities() { return m_entities.data; }
	wv::SlotMap<Entity*>& getEntitiesContainer() { return m_entities; }

	size_t getEntityIndex( Entity* _entity ) const { 
		for ( size_t i = 0; i < m_entities.data.size(); i++ )
			if ( _entity == m_entities.data[ i ] )
				return i;
		
		WV_ASSERT_MSG( false, "Entity does not exist here" );
		return 0;
	}

	size_t getEntityIndirectIndex( Entity* _entity ) const { 
		for ( size_t i = 0; i < m_entities.data.size(); i++ )
			if ( _entity == m_entities.data[ i ] )
				return m_entities.dataIDs[ i ];
		
		WV_ASSERT_MSG( false, "Entity does not exist here" );
		return 0;
	}

	IComponentContainer* container( int _compTypeIndex ) { return m_containers.at( _compTypeIndex ); }

private:
	std::unordered_map<int, IComponentContainer*> m_containers;
	wv::SlotMap<Entity*> m_entities;
	std::bitset<256> m_bitmask{};
};

class ISystem : public IUpdatable
{
public:
	std::vector<Archetype*>& getArchetypes() {
		return m_archetypes;
	}

	std::bitset<256> getArchetypeBitmask() const { return m_archetypeBitmask; }
	bool matchesBitmask( std::bitset<256> _bitmask ) const {
		return ( m_archetypeBitmask & _bitmask ) == m_archetypeBitmask;
	}

	virtual void configure( ArchetypeConfig& _config ) = 0;

	virtual void onComponentAdded( Archetype* _archetype, size_t _index ) { }
	virtual void onComponentRemoved( Archetype* _archetype, size_t _index ) { }

private:
	friend class ECSEngine;
	friend class World;

	std::bitset<256> m_archetypeBitmask{};
	std::vector<Archetype*> m_archetypes;
};

template<typename Ty>
inline int ECSEngine::registerComponentType()
{
	if ( ComponentTypeDef<Ty>::index == -1 )
	{
		ComponentTypeDef<Ty>::index = numComponentTypes++;
		m_componentVectorFuns[ ComponentTypeDef<Ty>::index ] = []()
			{
				return (IComponentContainer*)WV_NEW( ComponentContainer<Ty> );
			};
	}

	return ComponentTypeDef<Ty>::index;
}

template<typename Ty>
inline int ECSEngine::registerSystemType()
{
	if ( SystemTypeDef<Ty>::index == -1 )
	{
		SystemTypeDef<Ty>::index = numSystemTypes++;
		// allocator?
	}

	return SystemTypeDef<Ty>::index;
}

template<typename Ty>
Ty* ECSEngine::addSystem() 
{
	static_assert( std::is_base_of<ISystem, Ty>(), "Type must derive from wv::ISystem" );

	if( SystemTypeDef<Ty>::index == -1 )
		registerSystemType<Ty>();

	ISystem* s = WV_NEW_NAMED( Ty, "Ty : ISystem" );
	ArchetypeConfig config{};
	s->configure( config );

	s->m_archetypeBitmask = config.getBitmask();
	registerArchetype( config );

	m_systemIndexMap.emplace( SystemTypeDef<Ty>::index, s );
	return (Ty*)s;
}

template<typename Ty>
void ECSEngine::addComponent( Entity* _entity, const Ty& _component )
{
	WV_ASSERT_MSG( _entity != nullptr, "Entity cannot be nullptr" );

	Archetype* oldArchetype = _entity->archetype;
	size_t indirectIndex = 0;

	if ( oldArchetype )
		indirectIndex = oldArchetype->getEntityIndirectIndex( _entity );
	
	std::bitset<256> bitmask{};
	int compTypeIndex = ComponentTypeDef<Ty>::index;
	WV_ASSERT( compTypeIndex >= 0 );
	bitmask[ compTypeIndex ] = true;

	if ( oldArchetype )
	{
		if ( ( bitmask & oldArchetype->m_bitmask ).any() )
		{
			WV_LOG_ERROR( "Only one component per type per object\n" );
			return;
		}

		bitmask |= oldArchetype->m_bitmask;
	}

	Archetype* newArchetype = getExactArchetype( bitmask );

	if ( newArchetype == nullptr ) // new archetype, register
		newArchetype = registerArchetype( bitmask );
	
	// move other components over to the new archetype
	size_t newIndex = 0;
	if ( oldArchetype )
	{
		for ( auto& [compTypeIndex, oldContainer] : oldArchetype->m_containers )
		{
			IComponentContainer* newContainer = newArchetype->container( compTypeIndex );
			newIndex = newContainer->moveComponent( oldContainer, indirectIndex );
		}

		oldArchetype->m_entities.erase( indirectIndex );
	}

	newArchetype->m_entities.push( _entity );
	newArchetype->getComponentContainer<Ty>().push( _component );
	_entity->archetype = newArchetype;
}

template<typename Ty>
inline void ECSEngine::removeComponent( Entity* _entity )
{
	WV_ASSERT_MSG( _entity != nullptr, "Entity cannot be nullptr" );
	int index = ComponentTypeDef<Ty>::index;
	WV_ASSERT( index >= 0 );

	Archetype* oldArchetype = _entity->archetype;
	int indirectIndex = oldArchetype->getEntityIndirectIndex( _entity );

	if ( !oldArchetype->m_bitmask[ index ] )
	{
		WV_LOG_ERROR( "Entity does not have component\n" );
		return;
	}

	// create new bitmask for this archetype
	std::bitset<256> bitmask = oldArchetype->m_bitmask;
	bitmask[ index ] = false;

	if ( bitmask.any() )
	{
		Archetype* newArchetype = getExactArchetype( bitmask ); // find it
		if ( newArchetype == nullptr ) // new archetype, register
			newArchetype = registerArchetype( bitmask );

		// move other components to the new archetype
		
		for ( auto& [compTypeIndex, oldContainer] : oldArchetype->m_containers )
		{
			if ( compTypeIndex == index )
				continue;

			IComponentContainer* newContainer = newArchetype->container( compTypeIndex );
			newContainer->moveComponent( oldContainer, indirectIndex );
		}

		_entity->archetype = newArchetype;
		newArchetype->m_entities.push( _entity );
	}
	else
	{
		_entity->archetype = nullptr;
	}
	
	oldArchetype->container( index )->eraseComponent( indirectIndex );
	oldArchetype->m_entities.erase( indirectIndex );
}

}