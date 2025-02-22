#include "Model.h"

#include <wv/memory/memory.h>
#include <wv/scene/component/model_component.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::cModelObject::cModelObject( const UUID& _uuid, const std::string& _name ):
	Entity{ _uuid, _name }
{
}

///////////////////////////////////////////////////////////////////////////////////////

wv::cModelObject* wv::cModelObject::parseInstance( sParseData& _data )
{
	wv::Json& json = _data.json;

	wv::UUID    uuid = json[ "uuid" ].int_value();
	std::string name = json[ "name" ].string_value();

	wv::Json tfm = json[ "transform" ];
	Vector3f pos = jsonToVec3( tfm[ "pos" ].array_items() );
	Vector3f rot = jsonToVec3( tfm[ "rot" ].array_items() );
	Vector3f scl = jsonToVec3( tfm[ "scl" ].array_items() );

	Transformf transform;
	transform.setPosition( pos );
	transform.setRotation( rot );
	transform.setScale   ( scl );

	wv::Json data = json[ "data" ];
	std::string meshPath = data["path"].string_value();
	
	cModelObject* model = WV_NEW( wv::cModelObject, uuid, name );
	model->m_transform = transform;
	model->addComponent<ModelComponent>( meshPath );

	return model;
}

///////////////////////////////////////////////////////////////////////////////////////
