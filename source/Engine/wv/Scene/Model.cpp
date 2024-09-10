#include "Model.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Device/GraphicsDevice.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Memory/FileSystem.h>
#include <wv/Assets/Materials/Material.h>

#include <wv/Memory/ModelParser.h>

#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////

wv::cModelObject::cModelObject( const UUID& _uuid, const std::string& _name, sMeshNode* _meshNode ) :
	iSceneObject{ _uuid, _name },
	m_mesh{ _meshNode },
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
	if( m_mesh == nullptr )
	{
		if( m_meshPath == "" )
		{
			Debug::Print( Debug::WV_PRINT_WARN, "No mesh path provided, defaulting to cube\n" );
			m_meshPath = "res/meshes/cube";
		}

		wv::Parser parser;
		m_mesh = parser.load( m_meshPath.c_str(), app->m_pMaterialRegistry);
		m_transform.addChild( &m_mesh->transform );
	}
}

static void destroyMeshNode( wv::iGraphicsDevice* _graphics, wv::sMeshNode* _node )
{
	if( !_node )
		return;

	for( auto& mesh : _node->meshes )
		_graphics->destroyMesh( &mesh );
	
	for( auto& node : _node->children )
		destroyMeshNode( _graphics, node );
}

void wv::cModelObject::onUnloadImpl()
{
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	destroyMeshNode( device, m_mesh ); /// TODO: take care of meshes in resource registry
}

void wv::cModelObject::updateImpl( double _deltaTime )
{
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cModelObject::drawImpl( iDeviceContext* _context, iGraphicsDevice* _device )
{
	if( m_mesh )
		_device->drawNode( m_mesh );
}
