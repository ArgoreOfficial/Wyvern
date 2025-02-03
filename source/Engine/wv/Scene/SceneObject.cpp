#include "SceneObject.h"

///////////////////////////////////////////////////////////////////////////////////////

wv::IEntity::IEntity( const UUID& _uuid, const std::string& _name ):
	m_uuid{ _uuid },
	m_name{ _name }
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IEntity::~IEntity()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IEntity* wv::IEntity::getChildByUUID( const wv::UUID& _uuid )
{
	/*
	for( auto& child : m_children )
	{
		if( child->getUUID() == _uuid )
			return child;
	}

	for( auto& child : m_children )
	{
		IEntity* foundChild = child->getChildByUUID( _uuid );
		if( foundChild )
			return foundChild;
	}
	*/
	return nullptr;
}
