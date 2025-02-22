#include "Entity.h"

#include <wv/scene/component/script_component.h>
#include <wv/scene/component/icomponent.h>

wv::Entity::Entity( const UUID& _uuid, const std::string& _name ) :
	IEntity{ _uuid, _name }
{
	
}

wv::Entity::~Entity()
{
	for ( size_t i = 0; i < m_components.size(); i++ )
	{
		WV_FREE( m_components[ i ] );
		m_components[ i ] = nullptr;
	}

	m_components.clear();
}
