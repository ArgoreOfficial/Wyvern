#include "model_component.h"

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/mesh/mesh_resource.h>
#include <wv/memory/file_system.h>
#include <wv/material/material.h>

#include <wv/resource/resource_registry.h>

#include <wv/scene/entity/entity.h>

#include <fstream>

void wv::ModelComponent::onConstruct( void )
{
	wv::cEngine* app = wv::cEngine::get();

	if ( m_meshPath == "" )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "No mesh path provided, defaulting to cube\n" );
		m_meshPath = "meshes/cube.dae";
	}

	m_mesh = app->m_pResourceRegistry->load<cMeshResource>( m_meshPath )->createInstance();
	getParent()->m_transform.addChild(&m_mesh.transform);
}

void wv::ModelComponent::onDestruct( void )
{
	m_mesh.destroy();
}

void wv::ModelComponent::onDraw( wv::iDeviceContext* _context, wv::IGraphicsDevice* _device )
{
	m_mesh.draw();
}
