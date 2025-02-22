#include "mesh_resource.h"

#include <wv/memory/model_parser.h>
#include <wv/engine.h>
#include <wv/graphics/graphics_device.h>
#include <wv/Resource/resource_registry.h>
#include <wv/material/material.h>

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <wv/shader/shader_resource.h>
#include <wv/texture/texture_resource.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::MeshInstance::draw()
{
	if( pResource )
		pResource->addToDrawQueue( *this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::MeshInstance::destroy()
{
	if( pResource )
		pResource->destroyInstance( *this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::MeshResource::load( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	Engine* app = Engine::get();

	wv::Parser parser;
	m_pMeshNode = parser.load( m_path.c_str(), app->m_pResourceRegistry );
}

///////////////////////////////////////////////////////////////////////////////////////

static void unloadMeshNode( wv::MeshNode* _node )
{
	wv::IGraphicsDevice* pLowLevelGraphics = wv::Engine::get()->graphics;
	wv::ResourceRegistry* pResourceRegistry = wv::Engine::get()->m_pResourceRegistry;

	for ( auto& meshID : _node->meshes )
	{
		wv::Mesh mesh = pLowLevelGraphics->m_meshes.at( meshID );
		pResourceRegistry->unload( mesh.pMaterial );
		pLowLevelGraphics->destroyMesh( meshID );
	}
	
	for ( auto& child : _node->children )
		unloadMeshNode( child );
	
	WV_FREE( _node );
}

void wv::MeshResource::unload( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	if( m_pMeshNode )
		unloadMeshNode( m_pMeshNode );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::MeshInstance wv::MeshResource::createInstance()
{
	MeshInstance instance;
	instance.pResource = this;
	
    return instance;
}

void wv::MeshResource::destroyInstance( MeshInstance& _instance )
{
	Engine* app = Engine::get();
	app->m_pResourceRegistry->unload( _instance.pResource );

	_instance.pResource = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::MeshResource::addToDrawQueue( MeshInstance& _instance )
{
	std::scoped_lock lock{ m_mutex };

	m_drawQueue.push_back( _instance.transform );
}

