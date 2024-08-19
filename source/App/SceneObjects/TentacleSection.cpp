#include "TentacleSection.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Memory/MemoryDevice.h>
#include <wv/Assets/Materials/Material.h>

#include <wv/Memory/ModelParser.h>

#include <fstream>

#include "TentacleSettingWindow.h"

///////////////////////////////////////////////////////////////////////////////////////

cTentacleSectionObject::cTentacleSectionObject( const uint64_t& _uuid, const std::string& _name, wv::Mesh* _mesh ) :
	iSceneObject{ _uuid, _name },
	m_mesh{ _mesh }
{

}

///////////////////////////////////////////////////////////////////////////////////////

cTentacleSectionObject::~cTentacleSectionObject()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void cTentacleSectionObject::onLoadImpl()
{
	wv::cEngine* app = wv::cEngine::get();

	wv::assimp::Parser parser;
	m_mesh = parser.load( "res/meshes/debug-cube.dae", app->m_pMaterialRegistry );
}

///////////////////////////////////////////////////////////////////////////////////////

void cTentacleSectionObject::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	device->destroyMesh( &m_mesh );
}

///////////////////////////////////////////////////////////////////////////////////////

void cTentacleSectionObject::updateImpl( double _deltaTime )
{
	if( m_mesh )
	{
		sTentacleSetting x = cTentacleSettingWindowObject::settings[ 0 ];
		sTentacleSetting y = cTentacleSettingWindowObject::settings[ 1 ];
		sTentacleSetting z = cTentacleSettingWindowObject::settings[ 2 ];

		double t = wv::cEngine::get()->context->getTime();
		
		m_transform.setRotation( {
			sinf( ( float )t * x.frequency + x.phase ) * x.amplitude + x.shift,
			sinf( ( float )t * y.frequency + y.phase ) * y.amplitude + y.shift,
			sinf( ( float )t * z.frequency + z.phase ) * z.amplitude + z.shift 
		} );

		m_mesh->transform = m_transform;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void cTentacleSectionObject::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
	if( m_mesh )
		_device->draw( m_mesh );
}
