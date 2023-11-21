#include "cSceneGraph.h"
using namespace wv;

void wv::cSceneGraph::add( iSceneObject* _object )
{
	m_objects.push_back( _object );
	_object->setID( m_internalIDCount );
	m_internalIDCount++;
}

void wv::cSceneGraph::update( double _deltaTime )
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->update( _deltaTime );
	}
}

void wv::cSceneGraph::draw3D()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->draw3D();
	}
}

void wv::cSceneGraph::draw2D()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->draw2D();
	}
}

void wv::cSceneGraph::drawUI()
{
	for ( size_t i = 0; i < m_objects.size(); i++ )
	{
		m_objects[ i ]->drawUI();
	}
}
