#include "cMeshObject.h"

#include <wv/Managers/cContentManager.h>

wv::cMeshObject::cMeshObject( std::string _name ) : iSceneObject( _name )
{

}

wv::cMeshObject::~cMeshObject()
{
	delete m_model;
}

wv::cMeshObject* wv::cMeshObject::create( std::string _path )
{
	m_model = cContentManager::getInstance().getModel( _path );
	m_model->create( _path );
	m_model->transform.scale = { 0.008f, 0.008f, 0.008f }; // TODO: fix
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
