#include "ModelComponent.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Graphics/GraphicsDevice.h>

#include <wv/Mesh/MeshResource.h>
#include <wv/Memory/FileSystem.h>
#include <wv/Material/Material.h>

#include <wv/Resource/ResourceRegistry.h>

#include <wv/Scene/Entity/Entity.h>

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
