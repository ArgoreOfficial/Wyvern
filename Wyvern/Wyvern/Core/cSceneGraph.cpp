#include "cSceneGraph.h"
using namespace WV;

void WV::cSceneGraph::add( ISceneObject* _object )
{
	m_objects.push_back( _object );
	_object->setID( m_internalIDCount );
	m_internalIDCount++;
}

void WV::cSceneGraph::update( double _deltaTime )
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->update( _deltaTime );
	}
}

void WV::cSceneGraph::draw3D()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->draw3D();
	}
}

void WV::cSceneGraph::draw2D()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->draw2D();
	}
}

void WV::cSceneGraph::drawUI()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->drawUI();
	}
}
