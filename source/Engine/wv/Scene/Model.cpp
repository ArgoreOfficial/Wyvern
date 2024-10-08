#include "Model.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/Graphics.h>

#include <wv/Mesh/MeshResource.h>
#include <wv/Memory/FileSystem.h>
#include <wv/Material/Material.h>

#include <wv/Resource/ResourceRegistry.h>

#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////

wv::cModelObject::cModelObject( const UUID& _uuid, const std::string& _name, const std::string& _meshPath ):
	iSceneObject{ _uuid, _name },
	m_meshPath{ _meshPath }
{
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cModelObject::~cModelObject()
{
	
}

wv::cModelObject* wv::cModelObject::parseInstance( sParseData& _data )
{
	wv::Json& json = _data.json;

	wv::UUID    uuid = json[ "uuid" ].int_value();
	std::string name = json[ "name" ].string_value();

	wv::Json tfm = json[ "transform" ];
	cVector3f pos = jsonToVec3( tfm[ "pos" ].array_items() );
	cVector3f rot = jsonToVec3( tfm[ "rot" ].array_items() );
	cVector3f scl = jsonToVec3( tfm[ "scl" ].array_items() );

	Transformf transform;
	transform.setPosition( pos );
	transform.setRotation( rot );
	transform.setScale   ( scl );

	wv::Json data = json[ "data" ];
	std::string meshPath = data["path"].string_value();
	
	cModelObject* model = new wv::cModelObject( uuid, name, meshPath );
	model->m_transform = transform;
	return model;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cModelObject::onLoadImpl()
{
	wv::cEngine* app = wv::cEngine::get();

	if ( m_meshPath == "" )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "No mesh path provided, defaulting to cube\n" );
		m_meshPath = "res/meshes/cube.dae";
	}

	m_mesh = app->m_pResourceRegistry->load<cMeshResource>( m_meshPath )->createInstance();
	m_transform.addChild( &m_mesh.transform );
}

void wv::cModelObject::onUnloadImpl()
{
	m_mesh.destroy();
}

void wv::cModelObject::updateImpl( double _deltaTime )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cModelObject::drawImpl( iDeviceContext* _context, iLowLevelGraphics* _device )
{
	m_mesh.draw();
}
