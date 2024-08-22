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
	m_mesh{ _mesh },
	m_meshPath{ "" }
{

}

wv::cModelObject::cModelObject( const UUID& _uuid, const std::string& _name, const std::string& _meshPath ):
	iSceneObject{ _uuid, _name },
	m_mesh{ nullptr },
	m_meshPath{ _meshPath }
{
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cModelObject::~cModelObject()
{
	
}

wv::cModelObject* wv::cModelObject::createInstanceJson( nlohmann::json& _json )
{
	wv::UUID    uuid = _json.value( "uuid", cEngine::getUniqueUUID() );
	std::string name = _json.value( "name", "" );

	nlohmann::json tfm = _json[ "transform" ];
	std::vector<float> pos = tfm[ "pos" ].get<std::vector<float>>();
	std::vector<float> rot = tfm[ "rot" ].get<std::vector<float>>();
	std::vector<float> scl = tfm[ "scl" ].get<std::vector<float>>();

	Transformf transform;
	transform.setPosition( { pos[ 0 ], pos[ 1 ], pos[ 2 ] } );
	transform.setRotation( { rot[ 0 ], rot[ 1 ], rot[ 2 ] } );
	transform.setScale( { scl[ 0 ], scl[ 1 ], scl[ 2 ] } );

	nlohmann::json data = _json[ "data" ];
	std::string meshPath = data.value( "path", "" );
	
	cModelObject* model = new wv::cModelObject( uuid, name, meshPath );
	model->m_transform = transform;
	return model;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cModelObject::onLoadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	if( m_mesh == nullptr )
	{
		if( m_meshPath == "" )
		{
			Debug::Print( Debug::WV_PRINT_WARN, "No mesh path provided, defaulting to cube\n" );
			m_meshPath = "res/meshes/debug-cube.dae";
		}

		wv::assimp::Parser parser;
		m_mesh = parser.load( m_meshPath.c_str(), app->m_pMaterialRegistry);
	}
}

void wv::cModelObject::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	if( m_mesh )
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
