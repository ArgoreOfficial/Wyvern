#pragma once

#include <wv/entity/entity_component.h>

#include <set>
#include <vector>
#include <unordered_map>

namespace wv {

class Entity;

template<typename CompTy>
class EntityComponentContainer
{
public:
	EntityComponentContainer() = default;

	bool registerComponent( Entity* _entity, IEntityComponent* _component );
	bool unregisterComponent( Entity* _entity, IEntityComponent* _component );
	
	bool hasComponent( UUID _componentID ) const { return m_entityMap.contains( _componentID ); }
	Entity* getEntity( UUID _componentID ) const { return hasComponent( _componentID ) ? m_entityMap.at( _componentID ) : nullptr; }
	const std::vector<CompTy*>& getComponents() const { return m_registeredComponents; }

	std::vector<Entity*> getEntities() const {
		std::set<Entity*> entitiesSet;
		std::vector<Entity*> entities;
		
		for ( auto p : m_entityMap )
		{
			if ( !entitiesSet.contains( p.second ) )
			{
				entitiesSet.insert( p.second );
				entities.push_back( p.second );
			}
		}
		
		return entities;
	}

private:
	std::set<UUID> m_registeredComponentIDs;
	std::vector<CompTy*> m_registeredComponents;
	std::unordered_map<UUID, Entity*> m_entityMap;
};

template<typename CompTy>
inline bool EntityComponentContainer<CompTy>::registerComponent( Entity* _entity, IEntityComponent* _component )
{
	if ( _component == nullptr )
		return false;

	UUID id = _component->getID();
	if ( m_registeredComponentIDs.contains( id ) )
		return false; // already registered

	if ( CompTy* comp = tryCast<CompTy>( _component ) )
	{
		m_registeredComponentIDs.insert( id );
		m_registeredComponents.push_back( comp );

		if ( _entity != nullptr )
		{
			m_entityMap[ id ] = _entity;
		}
	}
	else
		return false;

	return true;
}

template<typename CompTy>
inline bool EntityComponentContainer<CompTy>::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	if ( _component == nullptr )
		return false;

	UUID id = _component->getID();
	if ( !m_registeredComponentIDs.contains( id ) )
		return false; // not registered

	if ( CompTy* comp = tryCast<CompTy>( _component ) )
	{
		m_registeredComponentIDs.erase( id );

		if ( _entity != nullptr )
			m_entityMap.erase( id );

		std::erase( m_registeredComponents, comp );
	}
	else
		return false;

	return true;
}


}