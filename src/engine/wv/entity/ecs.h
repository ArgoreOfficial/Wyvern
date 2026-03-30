#pragma once

#include <wv/entity/entity.h>
#include <wv/memory/memory.h>

#include <bitset>
#include <vector>
#include <unordered_map>

namespace wv {

class Entity;
class Archetype;
class ECSEngine;
class ISystem;

struct ArchetypeConfig;
struct IComponentVector;

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

	Archetype* registerArchetype( ArchetypeConfig& _config );
	Archetype* getExactArchetype( std::bitset<256> _bitmask );

	void updateSystems();
	
	template<typename Ty>
	void addComponent( Entity* _entity, const Ty& _component );

	template<typename Ty>
	void removeComponent( Entity* _entity );

	void removeAllComponents( Entity* _entity );

	int numComponentTypes = 0;
	int numSystemTypes = 0;

	std::vector<Archetype*> m_archetypes;
	std::vector<ISystem*> m_systems;
	std::unordered_map<int, ISystem*> m_systemIndexMap;

	std::unordered_map<int, std::function<IComponentVector* ( )>> m_componentVectorFuns;
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

struct IComponentVector
{
	virtual void moveComponent( IComponentVector* _oldVector, size_t _index ) = 0;
	virtual void eraseComponent( size_t _index ) = 0;
};

template<typename Ty>
struct ComponentVector : public IComponentVector
{
	std::vector<Ty> components;

	virtual void moveComponent( IComponentVector* _oldVector, size_t _index ) override
	{
		ComponentVector<Ty>* oldVector = reinterpret_cast<ComponentVector<Ty>*>( _oldVector );

		Ty comp = oldVector->components[ _index ];
		oldVector->components.erase( oldVector->components.begin() + _index );

		components.push_back( comp );
	}

	virtual void eraseComponent( size_t _index ) override
	{
		components.erase( components.begin() + _index );
	}
};

struct Archetype
{
	~Archetype() {
		for ( auto pair : m_vectors )
			WV_FREE( pair.second );

		m_vectors.clear();
		m_entities.clear();
	}

	template<typename Ty>
	std::vector<Ty>& getComponents() {
		int componentTypeIndex = ECSEngine::ComponentTypeDef<Ty>::index;
		WV_ASSERT_MSG( componentTypeIndex >= 0, "Component type not registered" );

		ComponentVector<Ty>* vec = reinterpret_cast<ComponentVector<Ty>*>( m_vectors.at( componentTypeIndex ) );
		return vec->components;
	}

	size_t getNumEntities() const { return m_entities.size(); }
	Entity* getEntity( size_t _index ) { return m_entities[ _index ]; }

	std::unordered_map<int, IComponentVector*> m_vectors;
	std::vector<Entity*> m_entities;
	std::bitset<256> m_bitmask{};
};

class ISystem
{
public:
	std::vector<Archetype*>& getArchetypes() {
		return m_archetypes;
	}

	std::bitset<256> getArchetypeBitmask() const { return m_archetypeBitmask; }

	virtual void configure( ArchetypeConfig& _config ) = 0;

	virtual void initialize() { }
	virtual void shutdown() { }

	virtual void update() = 0;

private:
	friend class ECSEngine;

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
				return (IComponentVector*)WV_NEW( ComponentVector<Ty> );
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

	ISystem* s = WV_NEW_NAMED( Ty, "Ty : ISystem" );
	ArchetypeConfig config{};
	s->configure( config );

	s->m_archetypeBitmask = config.getBitmask();
	registerArchetype( config );

	m_systems.push_back( s );
	m_systemIndexMap.emplace( SystemTypeDef<Ty>::index, s );
	return (Ty*)s;
}

template<typename Ty>
void ECSEngine::addComponent( Entity* _entity, const Ty& _component )
{
	WV_ASSERT_MSG( _entity != nullptr, "Entity cannot be nullptr" );

	Archetype* oldArchetype = _entity->archetype;
	int oldArchetypeIndex = _entity->archetypeIndex;

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

	if ( newArchetype == nullptr )
	{
		// new archetype, register

		ArchetypeConfig config{};
		config.addComponentType<Ty>();
		if ( oldArchetype )
		{
			for ( auto& [tindex, vec] : oldArchetype->m_vectors )
				config.addComponentType( tindex );
		}
		
		newArchetype = registerArchetype( config );
	}

	// move other components over to the new archetype
	if ( oldArchetype )
	{
		for ( auto& [tindex, vec] : oldArchetype->m_vectors )
			newArchetype->m_vectors[ tindex ]->moveComponent( vec, oldArchetypeIndex );

		oldArchetype->m_entities.erase( oldArchetype->m_entities.begin() + oldArchetypeIndex );

		for ( auto e : oldArchetype->m_entities )
			e->archetypeIndex--;
	}

	newArchetype->getComponents<Ty>().push_back( _component );
	newArchetype->m_entities.push_back( _entity );

	_entity->archetype = newArchetype;
	_entity->archetypeIndex = newArchetype->m_entities.size() - 1;
}

template<typename Ty>
inline void ECSEngine::removeComponent( Entity* _entity )
{
	WV_ASSERT_MSG( _entity != nullptr, "Entity cannot be nullptr" );
	int index = ComponentTypeDef<Ty>::index;
	WV_ASSERT( index >= 0 );

	Archetype* oldArchetype = _entity->archetype;
	int oldArchetypeIndex   = _entity->archetypeIndex;

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

		// move other components to the new archetype
		for ( auto& [compTypeIndex, oldCompVec] : oldArchetype->m_vectors )
		{
			if ( compTypeIndex == index )
				continue;

			newArchetype->m_vectors[ compTypeIndex ]->moveComponent( oldCompVec, oldArchetypeIndex );
		}

		_entity->archetype = newArchetype;
		_entity->archetypeIndex = newArchetype->m_entities.size() - 1;
	}
	else
	{
		_entity->archetype = nullptr;
		_entity->archetypeIndex = 0;
	}
	
	oldArchetype->m_vectors[ index ]->eraseComponent( oldArchetypeIndex );
	oldArchetype->m_entities.erase( oldArchetype->m_entities.begin() + oldArchetypeIndex );

	for ( auto e : oldArchetype->m_entities )
		e->archetypeIndex--;
}

}