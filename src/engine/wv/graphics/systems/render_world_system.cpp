#include "render_world_system.h"

#include <wv/graphics/components/mesh_component.h>

void wv::RenderWorldSystem::initialize()
{
	
}

void wv::RenderWorldSystem::shutdown()
{
	
}

void wv::RenderWorldSystem::registerComponent( Entity* _entity, IEntityComponent* _component )
{
	MeshComponent* meshComponent = tryCast<MeshComponent>( _component );
	if ( meshComponent == nullptr )
		return;

	for ( auto registeredComponent : m_registeredMeshComponents )
		if ( meshComponent == registeredComponent )
			return;
	
	m_registeredMeshComponents.push_back( meshComponent );
}

void wv::RenderWorldSystem::unregisterComponent( Entity* _entity, IEntityComponent* _component )
{
	MeshComponent* meshComponent = tryCast<MeshComponent>( _component );
	if ( meshComponent == nullptr )
		return;

	for ( size_t i = 0; i < m_registeredMeshComponents.size(); i++ )
	{
		if ( meshComponent != m_registeredMeshComponents[ i ] )
			continue;

		m_registeredMeshComponents.erase( m_registeredMeshComponents.begin() + i );
		break;
	}

}

void wv::RenderWorldSystem::update( double _deltaTime )
{
	// printf( "TestWorldSystem updated with dt %f\n", _deltaTime );
}
