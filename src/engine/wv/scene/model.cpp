#include "Model.h"

#include <wv/memory/memory.h>
#include <wv/scene/component/model_component.h>

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/mesh/mesh_resource.h>
#include <wv/filesystem/file_system.h>
#include <wv/material/material.h>

#include <wv/resource/resource_registry.h>

#include <wv/scene/entity/entity.h>

#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////

wv::ModelObject::ModelObject( const UUID& _uuid, const std::string& _name ):
	Entity{ _uuid, _name }
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::ModelObject::onConstruct( void )
{
	wv::Engine* app = wv::Engine::get();

	if ( m_meshPath == "" )
	{
		Debug::Print( Debug::WV_PRINT_WARN, "No mesh path provided, defaulting to cube\n" );
		m_meshPath = "meshes/cube.dae";
	}

	app->m_pResourceRegistry->load<MeshResource>( m_meshPath )->makeInstance( &m_mesh );
	m_transform.addChild( &m_mesh.transform );
}

void wv::ModelObject::onDestruct( void )
{
	m_mesh.destroy();
}

///////////////////////////////////////////////////////////////////////////////////////
