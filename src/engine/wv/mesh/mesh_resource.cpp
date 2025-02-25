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

#include <wv/job/job_system.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::MeshInstance::draw()
{
	//if( pResource )
	//	pResource->addToDrawQueue( *this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::MeshInstance::destroy()
{
	if ( pResource )
		pResource->removeInstance( this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::MeshResource::load( FileSystem* /*_pFileSystem*/, IGraphicsDevice* /*_pGraphicsDevice*/ )
{
	Engine* app = Engine::get();
	wv::Parser parser;
	m_pMeshNode = parser.load( m_path.c_str(), app->m_pResourceRegistry );
}

///////////////////////////////////////////////////////////////////////////////////////

static void unloadMeshNode( wv::MeshNode* _node, wv::IGraphicsDevice* _pGraphicsDevice )
{
	wv::ResourceRegistry* pResourceRegistry = wv::Engine::get()->m_pResourceRegistry;

	for ( auto& meshID : _node->meshes )
	{
		wv::Mesh mesh = _pGraphicsDevice->m_meshes.at( meshID );
		pResourceRegistry->unload( mesh.pMaterial );
		_pGraphicsDevice->destroyMesh( meshID );
	}
	
	for ( auto& child : _node->children )
		unloadMeshNode( child, _pGraphicsDevice );
	
	WV_FREE( _node );
}

void wv::MeshResource::unload( FileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice )
{
	if( m_pMeshNode )
	{
		JobSystem* pJobSystem = Engine::get()->m_pJobSystem;
		MeshNode*  pMeshNode  = m_pMeshNode; // lambdas can only capture local variables
		Job::JobFunction_t fptr = [=]( void* _pUserData )
			{
				unloadMeshNode( pMeshNode, _pGraphicsDevice );
			};

		Job* job = pJobSystem->createJob( 
			JobThreadType::kRENDER, 
			Engine::get()->m_pResourceRegistry->getResourceFence(), // ew
			nullptr, 
			fptr, 
			m_pMeshNode );
		pJobSystem->submit( { job } );
	}
	
	m_mutex.lock();
	if ( m_instances.size() > 0 )
		Debug::Print( Debug::WV_PRINT_ERROR, "Mesh %s has %zu instances left", m_name, m_instances.size() );
	
	for ( auto& instance : m_instances )
		WV_FREE( instance );
	
	m_instances.clear();
	m_mutex.unlock();
}

void wv::MeshResource::makeInstance( MeshInstance* _instance )
{
	_instance->pResource = this;

	m_mutex.lock();
	m_instances.push_back( _instance );
	m_mutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::MeshResource::removeInstance( MeshInstance* _instance )
{
	Engine* app = Engine::get();
	
	m_mutex.lock();
	for ( auto itr = m_instances.begin(); itr != m_instances.end(); itr++ )
	{
		if ( *itr != _instance )
			continue;

		_instance->pResource = nullptr;
		m_instances.erase( itr );
		break;
	}
	m_mutex.unlock();

	app->m_pResourceRegistry->unload( this );
}
