#include "Entity.h"

#include <wv/Scene/Component/ScriptComponent.h>

wv::Entity::Entity( const UUID& _uuid, const std::string& _name ) :
	IEntity{ _uuid, _name }
{
	addComponent<ScriptComponent>();
}
