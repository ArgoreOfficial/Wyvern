#include "Model.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Assets/Materials/Material.h>

#include <wv/Memory/ModelParser.h>

#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////

wv::cModelObject::cModelObject( const UUID& _uuid, const std::string& _name, Mesh* _mesh ) :
	iSceneObject{ _uuid, _name },
	m_mesh{ _mesh }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::cModelObject::~cModelObject()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cModelObject::onLoadImpl()
{
	wv::cEngine* app = wv::cEngine::get();

	wv::assimp::Parser parser;
	m_mesh = parser.load( "res/meshes/debug-cube.dae", app->m_pMaterialRegistry );
}

void wv::cModelObject::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	device->destroyMesh( &m_mesh );
}

void wv::cModelObject::updateImpl( double _deltaTime )
{
	if( m_mesh )
		m_mesh->transform = m_transform;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cModelObject::drawImpl( iDeviceContext* _context, iGraphicsDevice* _device )
{
	if( m_mesh )
		_device->draw( m_mesh );
}
