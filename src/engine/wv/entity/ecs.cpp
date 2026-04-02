#include "ecs.h"

wv::ECSEngine::~ECSEngine()
{
	for ( Archetype* archetype : m_archetypes )
		WV_FREE( archetype );

	for ( auto& [i,s] : m_systemIndexMap )
		WV_FREE( s );

	m_archetypes.clear();
	m_systemIndexMap.clear();
}

wv::Archetype* wv::ECSEngine::registerArchetype( ArchetypeConfig& _config )
{
	auto bitmask = _config.getBitmask();
	if ( bitmask.none() )
		return nullptr; // empty config

	Archetype* archetype = getExactArchetype( bitmask );

	if ( archetype )
		return archetype; // exact match found, archetype already exists

	// create new archetype

	archetype = WV_NEW( Archetype );
	archetype->m_bitmask = bitmask;

	for ( size_t i = 0; i < _config.componentTypeIndices.size(); i++ )
	{
		archetype->m_containers.emplace(
			_config.componentTypeIndices[ i ],
			m_componentVectorFuns[ _config.componentTypeIndices[ i ] ]()
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

wv::Archetype* wv::ECSEngine::registerArchetype( std::bitset<256> _bitmask )
{
	ArchetypeConfig config{};
	
	for ( size_t i = 0; i < numComponentTypes; i++ )
		if( _bitmask[ i ] )
			config.addComponentType( i );
	
	return registerArchetype( config );
}

void wv::ECSEngine::updateSystemsArchetypes()
{
	for ( auto& [i, s] : m_systemIndexMap )
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
	}
}

void wv::ECSEngine::removeAllComponents( Entity* _entity )
{
	Archetype* archetype = _entity->archetype;
	int indirectIndex = archetype->getEntityIndirectIndex( _entity );

	_entity->archetype = nullptr;

	for ( auto v : archetype->m_containers )
		v.second->eraseComponent( indirectIndex );

	archetype->m_entities.erase( indirectIndex );
}

void wv::ArchetypeConfig::addComponentType( int _typeIndex )
{
	for ( size_t i = 0; i < componentTypeIndices.size(); i++ )
		if ( componentTypeIndices[ i ] == _typeIndex )
			return;

	componentTypeIndices.push_back( _typeIndex );
}
