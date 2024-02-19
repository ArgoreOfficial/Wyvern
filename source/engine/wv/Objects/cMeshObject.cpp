#include "cMeshObject.h"

#include <wv/Managers/cContentManager.h>

wv::cMeshObject::cMeshObject( std::string _name ) : iSceneObject( _name )
{

}

wv::cMeshObject* wv::cMeshObject::create( std::string _path )
{
	m_model = cContentManager::getInstance().loadModel( _path );
	return this;
}

void wv::cMeshObject::update( double _delta_time )
{
	
}

void wv::cMeshObject::render()
{
	if( m_model )
		m_model->render();
}
