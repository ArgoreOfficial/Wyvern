#include "SceneObject.h"

///////////////////////////////////////////////////////////////////////////////////////

wv::iSceneObject::iSceneObject( const UUID& _uuid, const std::string& _name ):
	m_uuid{ _uuid },
	m_name{ _name }
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

wv::iSceneObject::~iSceneObject()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

wv::iSceneObject* wv::iSceneObject::getChildByUUID( const wv::UUID& _uuid )
{/*
	for( auto& child : m_children )
	{
		if( child->getUUID() == _uuid )
			return child;
	}

	for( auto& child : m_children )
	{
		iSceneObject* foundChild = child->getChildByUUID( _uuid );
		if( foundChild )
			return foundChild;
	}

 */
	return nullptr;
}
