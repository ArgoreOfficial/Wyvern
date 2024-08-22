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

cTentacleSectionObject* cTentacleSectionObject::createInstanceJson( nlohmann::json& _json )
{

	wv::UUID    uuid = _json.value( "uuid", wv::cEngine::getUniqueUUID() );
	std::string name = _json.value( "name", "" );

	nlohmann::json tfm = _json[ "transform" ];
	std::vector<float> pos = tfm[ "pos" ].get<std::vector<float>>();
	std::vector<float> rot = tfm[ "rot" ].get<std::vector<float>>();
	std::vector<float> scl = tfm[ "scl" ].get<std::vector<float>>();

	wv::Transformf transform;
	transform.setPosition( { pos[ 0 ], pos[ 1 ], pos[ 2 ] } );
	transform.setRotation( { rot[ 0 ], rot[ 1 ], rot[ 2 ] } );
	transform.setScale( { scl[ 0 ], scl[ 1 ], scl[ 2 ] } );

	nlohmann::json data = _json[ "data" ];
	std::string meshPath = data.value( "path", "" );

	cTentacleSectionObject* obj = new cTentacleSectionObject( uuid, name, 25.0f );
	obj->m_transform = transform;
	return obj;
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
			x.getValue( t ),
			y.getValue( t ),
			z.getValue( t ) 
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
