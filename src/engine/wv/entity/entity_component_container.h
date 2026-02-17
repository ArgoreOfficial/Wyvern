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
	
	bool hasComponent( UUID _UUID ) const { return m_entityMap.contains( _UUID ); }
	Entity* getEntity( UUID _UUID ) const { return hasComponent( _UUID ) ? m_entityMap.at( _UUID ) : nullptr; }
	const std::vector<CompTy*>& getComponents() const { return m_registeredComponents; }

private:
	std::set<UUID> m_registeredUUIDs;
	std::vector<CompTy*> m_registeredComponents;
	std::unordered_map<UUID, Entity*> m_entityMap;
};

template<typename CompTy>
inline bool EntityComponentContainer<CompTy>::registerComponent( Entity* _entity, IEntityComponent* _component )
{
	if ( _component == nullptr )
		return false;

	UUID id = _component->getID();
	if ( m_registeredUUIDs.contains( id ) )
		return false; // already registered

	if ( CompTy* comp = tryCast<CompTy>( _component ) )
	{
		m_registeredUUIDs.insert( id );
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
	if ( !m_registeredUUIDs.contains( id ) )
		return false; // not registered

	if ( CompTy* comp = tryCast<CompTy>( _component ) )
	{
		m_registeredUUIDs.erase( id );

		if ( _entity != nullptr )
			m_entityMap.erase( id );

		for ( auto it = m_registeredComponents.begin(); it != m_registeredComponents.end(); it++ )
		{
			if ( comp != *it )
				continue;
			m_registeredComponents.erase( it );
			break;
		}
	}
	else
		return false;

	return true;
}


}