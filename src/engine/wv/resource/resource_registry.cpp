#include "resource_registry.h"

#include <wv/material/material.h>
#include <wv/debug/log.h>
#include <wv/resource/resource.h>
#include <wv/renderer/mesh_renderer.h>
#include <wv/mesh/mesh_resource.h>

#include <vector>

wv::ResourceRegistry::~ResourceRegistry()
{
	if( !m_resources.empty() )
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Resource Registry has %i unloaded resources\n", (int)m_resources.size() );

	std::vector<std::string> markedForDelete;

	for ( auto& res : m_resources )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "  '%s'\n", res.first.c_str() );
		markedForDelete.push_back( res.second->getName() );
	}

	for ( size_t i = 0; i < markedForDelete.size(); i++ )
	{
		removeResource( markedForDelete[ i ] );

		wv::Debug::Print( wv::Debug::WV_PRINT_WARN, "Force unloaded '%s'\n", markedForDelete[ i ].c_str());
	}

	m_pJobSystem->deleteFence( m_resourceFence );
}

void wv::ResourceRegistry::drawMeshInstances()
{
	IMeshRenderer meshRenderer{ m_pGraphicsDevice };
	for ( auto& meshRes : m_meshes )
		meshRenderer.drawMesh( meshRes );
}

wv::IResource* wv::ResourceRegistry::getLoadedResource( const std::string& _name )
{
	wv::IResource* res = nullptr;
	
	auto search = m_resources.find( _name );
	if ( search != m_resources.end() )
		res = m_resources[ _name ];
	
	return res;
}

void wv::ResourceRegistry::addResource( IResource* _resource )
{
	std::string name = _resource->getName();
	if ( getLoadedResource( name ) )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Resource of name '%s' already exists\n", name.c_str() );
		return;
	}

	m_resources[ name ] = _resource;
}

void wv::ResourceRegistry::findAndRemoveResource( IResource* _resource )
{
	if ( !_resource )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Cannot unload null resource\n" );
		return;
	}

	std::string key = _resource->getName();
	removeResource( key );
}

void wv::ResourceRegistry::removeResource( const std::string& _name )
{
	std::scoped_lock lock{ m_mutex };

	auto search = m_resources.find( _name );
	if ( search == m_resources.end() )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Cannot unload resource '%s'. It does not exist\n", _name.c_str() );
		return;
	}

	for ( size_t i = 0; i < m_meshes.size(); i++ )
	{
		if ( m_meshes[ i ]->getName() != _name )
			continue;

		m_meshes.erase( m_meshes.begin() + i );
		i--;
		break;
	}

	//delete m_resources[ _name ];
	WV_FREE( m_resources[ _name ] );
	m_resources.erase( _name );
}
