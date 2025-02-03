#include "ResourceRegistry.h"

#include <wv/Material/Material.h>
#include <wv/Debug/Print.h>
#include <wv/Resource/Resource.h>
#include <wv/Mesh/MeshResource.h>
#include <vector>

wv::cResourceRegistry::~cResourceRegistry()
{
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
}

void wv::cResourceRegistry::initializeEmbeded()
{
	// anything loaded in here will always have numUsers of greater than 0
	// essentially keeping them alive for the enire application lifetime

	//load<cMaterial>( "DefaultMaterial.wmat" );
	//load<cMaterial>( "DebugMaterial.wmat" );
	//load<cMaterial>( "DebugTextureMaterial.wmat" );
}

void wv::cResourceRegistry::drawMeshInstances()
{
	for ( auto& meshRes : m_meshes )
		meshRes->drawInstances( m_pLowLevelGraphics );
}

wv::iResource* wv::cResourceRegistry::getLoadedResource( const std::string& _name )
{
	wv::iResource* res = nullptr;
	
	auto search = m_resources.find( _name );
	if ( search != m_resources.end() )
		res = m_resources[ _name ];
	
	return res;
}

void wv::cResourceRegistry::addResource( iResource* _resource )
{
	std::string name = _resource->getName();
	if ( getLoadedResource( name ) )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Resource of name '%s' already exists\n", name.c_str() );
		return;
	}

	m_resources[ name ] = _resource;
}

void wv::cResourceRegistry::findAndRemoveResource( iResource* _resource )
{
	if ( !_resource )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Cannot unload null resource\n" );
		return;
	}

	std::string key = _resource->getName();
	removeResource( key );
}

void wv::cResourceRegistry::removeResource( const std::string& _name )
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
