#include "TentacleSection.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Assets/Materials/Material.h>

#include <wv/Memory/ModelParser.h>
#include <wv/Memory/MemoryDevice.h>

#include <wv/Physics/PhysicsEngine.h>

#include <fstream>

#include "TentacleSettingWindow.h"

///////////////////////////////////////////////////////////////////////////////////////

cTentacleSectionObject::cTentacleSectionObject( const uint64_t& _uuid, const std::string& _name, float _segmentLength ) :
	iSceneObject{ _uuid, _name },
	m_segmentLength{_segmentLength}
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
		if( m_uuid == 0 )
		{
			m_transform = wv::cEngine::get()->m_pPhysicsEngine->getPhysicsBodyTransform( 0 );
		}
		else
		{
			sTentacleSetting x = cTentacleSettingWindowObject::settings[ 0 ];
			sTentacleSetting y = cTentacleSettingWindowObject::settings[ 1 ];
			sTentacleSetting z = cTentacleSettingWindowObject::settings[ 2 ];

			double t = wv::cEngine::get()->context->getTime();
		
			m_transform.setRotation( {
				x.getValue( t ),
				y.getValue( t ),
				z.getValue( t ) 
			} );

		}
		m_mesh->transform = m_transform;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void cTentacleSectionObject::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
	if( m_mesh )
		_device->draw( m_mesh );
}
