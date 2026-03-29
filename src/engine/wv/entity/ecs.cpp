#include "ecs.h"

wv::Archetype* wv::ECSEngine::registerArchetype( ArchetypeConfig& _config )
{
	auto bitmask = _config.getBitmask();
	if ( bitmask.none() )
		return nullptr; // empty config

	for ( size_t i = 0; i < m_archetypes.size(); i++ )
	{
		if ( m_archetypes[ i ]->m_bitmask != bitmask )
			continue;

		_config.freeContainers();
		return nullptr; // exact match found, archetype already exists
	}

	// create new archetype

	Archetype* archetype = WV_NEW( Archetype );
	archetype->m_bitmask = bitmask;

	for ( size_t i = 0; i < _config.componentTypeIndices.size(); i++ )
	{
		// give ownership of the container 
		archetype->m_vectors.emplace(
			_config.componentTypeIndices[ i ],
			_config.componentContainers[ i ]
		);
	}

	m_archetypes.push_back( archetype );
	return archetype;
}

wv::Archetype* wv::ECSEngine::getExactArchetype( std::bitset<256> _bitmask )
{
	for ( auto a : m_archetypes )
	{
		if ( a->m_bitmask == _bitmask )
			return a;
	}

	return nullptr;
}

void wv::ECSEngine::updateSystems()
{
	for ( ISystem* s : m_systems )
	{
		s->m_archetypes.clear();

		std::bitset<256> bitmask = s->getArchetypeBitmask();
		if ( bitmask.any() )
		{
			for ( auto& archetype : m_archetypes )
			{
				if ( archetype->getNumEntities() == 0 )
					continue; // skip if empty 

				if ( ( archetype->m_bitmask & bitmask ) == bitmask )
					s->m_archetypes.push_back( archetype );
			}
		}

		s->update();
	}
}

void wv::ECSEngine::updateEntityArchetype( Entity* _entity )
{
}

void wv::Archetype::moveComponents( Archetype* _oldArchetype, size_t _index )
{
	for ( auto pair : _oldArchetype->m_vectors )
		m_vectors[ pair.first ]->moveComponent( pair.second, _index );

	_oldArchetype->m_entities.erase( _oldArchetype->m_entities.begin() + _index );

	for ( auto e : _oldArchetype->m_entities )
		e->archetypeIndex--;
}
