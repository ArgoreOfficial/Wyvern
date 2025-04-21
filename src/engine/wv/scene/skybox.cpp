#include "skybox.h"

#include <wv/engine.h>
#include <wv/device/device_context.h>
#include <wv/graphics/graphics_device.h>

#include <wv/mesh/mesh_resource.h>
#include <wv/resource/resource_registry.h>

#include <wv/renderer/mesh_renderer.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::SkyboxObject::SkyboxObject( const UUID& _uuid, const std::string& _name ) :
	Entity{ _uuid, _name }
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::SkyboxObject::~SkyboxObject()
{
	
}

void wv::SkyboxObject::onConstruct()
{
	wv::Engine* app = wv::Engine::get();
	
	MeshResource* skysphere = app->m_pResourceRegistry->load<MeshResource>( "meshes/skysphere.glb" );
	skysphere->makeInstance( &m_mesh );

	m_transform.addChild( &m_mesh.transform );
}

void wv::SkyboxObject::onDestruct()
{
	m_mesh.destroy();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SkyboxObject::onUpdate( double /*_deltaTime*/ )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::SkyboxObject::onDraw( IDeviceContext* /*_context*/, IGraphicsDevice* /*_device*/ )
{
	wv::Engine::get()->m_pMeshRenderer->add( m_mesh.pResource );
}
