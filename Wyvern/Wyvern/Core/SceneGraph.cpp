#include "SceneGraph.h"
using namespace WV;

void WV::SceneGraph::add( ISceneObject* _object )
{
	m_objects.push_back( _object );
	_object->setID( m_internalIDCount );
	m_internalIDCount++;
}

void WV::SceneGraph::update( double _deltaTime )
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->update( _deltaTime );
	}
}

void WV::SceneGraph::draw3D()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->draw3D();
	}
}

void WV::SceneGraph::draw2D()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->draw2D();
	}
}

void WV::SceneGraph::drawUI()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->drawUI();
	}
}
