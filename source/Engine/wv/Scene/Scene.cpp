#include "Scene.h"

#include <wv/Memory/Memory.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::addChild( IEntity* _node, bool _triggerLoadAndCreate )
{
	if ( !_node )
		return;

	for ( size_t i = 0; i < m_entities.size(); i++ )
		if ( m_entities[ i ] == _node ) return; // node already has child
	
	m_entities.push_back( _node );

	if ( _triggerLoadAndCreate )
	{
		_node->onEnterImpl();
		_node->onConstructImpl();
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::removeChild( IEntity* _node )
{
	if ( !_node )
		return;

	for ( size_t i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] != _node )
			continue;

		m_entities.erase( m_entities.begin() + i );
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::destroyAllEntities()
{
	for( size_t i = 0; i < m_entities.size(); i++ )
		WV_FREE( m_entities[ i ] );

	m_entities.clear();
}

void wv::Scene::onUpdate( double _deltaTime )
{
	for ( size_t i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ]->m_transform.pParent != nullptr )
			continue;

		m_entities[ i ]->m_transform.update( nullptr );
	}
}

