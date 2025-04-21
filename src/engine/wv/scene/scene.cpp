#include "scene.h"

#include <wv/memory/memory.h>
#include <functional>

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::addChild( IEntity* _node, bool _triggerLoadAndCreate )
{
	if ( !_node )
		return;

	for ( size_t i = 0; i < m_entities.size(); i++ )
		if ( m_entities[ i ] == _node ) return; // node already has child
	
	registerUpdatable( _node );
	m_entities.push_back( _node );
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

void wv::Scene::onConstruct()
{
	for ( auto u : m_updatables )
		u->callOnConstruct();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::onDestruct()
{
	for ( auto u : m_updatables )
		u->callOnDestruct();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::onEnter()
{
	for ( auto u : m_updatables )
		u->callOnEnter();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::onExit()
{
	for ( auto u : m_updatables )
		u->callOnExit();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::onUpdate( double _deltaTime )
{
	for ( auto u : m_updatables )
		u->callOnUpdate( _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::onPhysicsUpdate( double _deltaTime )
{
	for ( auto u : m_updatables )
		u->callOnPhysicsUpdate( _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::onDraw( IDeviceContext* _pContext, IGraphicsDevice* _pDevice )
{
	for ( auto u : m_updatables )
		u->callOnDraw( _pContext, _pDevice );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Scene::destroyAllEntities()
{
	for( size_t i = 0; i < m_entities.size(); i++ )
		WV_FREE( m_entities[ i ] );

	m_entities.clear();
}

void wv::Scene::onUpdateTransforms()
{
	for ( size_t i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ]->m_transform.pParent != nullptr )
			continue;

		m_entities[ i ]->m_transform.update( nullptr );
	}
}

void wv::Scene::_addQueued()
{
	while ( !m_addedUpdatableQueue.empty() )
	{
		IUpdatable* u = m_addedUpdatableQueue.front();
		m_addedUpdatableQueue.pop();

		m_updatables.push_back( u );
	}
}

void wv::Scene::_removeQueued()
{
	while ( !m_removedUpdatableQueue.empty() )
	{
		IUpdatable* u = m_removedUpdatableQueue.front();
		m_removedUpdatableQueue.pop();

		std::vector<IUpdatable*>::iterator itr;
		for ( itr = m_updatables.begin(); itr != m_updatables.end(); itr++ )
		{
			if ( *itr != u )
				continue;

			m_updatables.erase( itr );
			break;
		}
	}
}

