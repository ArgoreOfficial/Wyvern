#include "ComponentSystem.h"

void WV::ComponentSystem::start()
{
	for ( size_t i = 0; i < m_components.size(); i++ )
	{
		m_components[ i ]->start();
	}
}

void WV::ComponentSystem::update( float _deltaTime )
{
	for ( size_t i = 0; i < m_components.size(); i++ )
	{
		m_components[ i ]->update( _deltaTime );
	}
}

void WV::ComponentSystem::exit()
{
	for ( size_t i = 0; i < m_components.size(); i++ )
	{
		m_components[ i ]->exit();
	}
}
