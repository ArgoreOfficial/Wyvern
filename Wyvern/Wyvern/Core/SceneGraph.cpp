#include "SceneGraph.h"
using namespace WV;

void SceneGraph::add( ISceneObject* _object )
{
	m_objects.push_back( _object );
	_object->setID( m_internalIDCount );
	m_internalIDCount++;
}
