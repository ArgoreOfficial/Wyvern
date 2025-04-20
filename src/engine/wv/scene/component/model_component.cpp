#include "model_component.h"

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/mesh/mesh_resource.h>
#include <wv/filesystem/file_system.h>
#include <wv/material/material.h>

#include <wv/resource/resource_registry.h>
#include <wv/renderer/mesh_renderer.h>

#include <wv/scene/entity/entity.h>

#include <fstream>

void wv::ModelComponent::onConstruct( void )
{
	wv::Engine* app = wv::Engine::get();

	if ( m_meshPath == "" )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "No mesh path provided, defaulting to cube\n" );
		m_meshPath = "meshes/cube.dae";
	}

	app->m_pResourceRegistry->load<MeshResource>( m_meshPath )->makeInstance( &m_mesh );
	getParent()->m_transform.addChild( &m_mesh.transform );
}

void wv::ModelComponent::onDestruct( void )
{
	m_mesh.destroy();
}

void wv::ModelComponent::onDraw( wv::IDeviceContext* _context, wv::IGraphicsDevice* _device )
{
	wv::Engine::get()->m_pMeshRenderer->add( m_mesh.pResource );
}
