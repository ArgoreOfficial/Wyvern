#include "ResourceRegistry.h"

#include <wv/Debug/Print.h>
#include <wv/Resource/Resource.h>

#include <vector>

wv::iResourceRegistry::~iResourceRegistry()
{
	wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "%s has %i unloaded resources\n", m_name.c_str(), (int)m_resources.size() );

	std::vector<std::string> markedForDelete;

	for ( auto& res : m_resources )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "  '%s'\n", res.first.c_str() );
		markedForDelete.push_back( res.second->getName() );
	}

	for ( int i = 0; i < markedForDelete.size(); i++ )
	{
		unloadResource( markedForDelete[ i ] );

		wv::Debug::Print( wv::Debug::WV_PRINT_WARN, "Force unloaded '%s'\n", markedForDelete[ i ].c_str());
	}
}

wv::iResource* wv::iResourceRegistry::getLoadedResource( const std::string& _name )
{
	if ( m_resources.contains( _name ) )
		return m_resources[ _name ];

	return nullptr;
}

void wv::iResourceRegistry::addResource( iResource* _resource )
{
	std::string name = _resource->getName();
	if ( getLoadedResource( name ) )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Resource of name '%s' already exists\n", name.c_str() );
		return;
	}
	
	m_resources[ name ] = _resource;
}

void wv::iResourceRegistry::update()
{
	if ( m_resourceLoader.isLoading() )
		m_resourceLoader.update();
}

void wv::iResourceRegistry::findAndUnloadResource( iResource* _resource )
{
	if ( !_resource )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Cannot unload null resource\n" );
		return;
	}

	std::string key = _resource->getName();
	unloadResource( key );
}

void wv::iResourceRegistry::unloadResource( const std::string& _name )
{
	if ( !m_resources.contains( _name ) )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Cannot unload shader '%s'. It does not exist\n", _name.c_str() );
		return;
	}

	m_resources[ _name ]->decrNumUsers();

	if ( m_resources[ _name ]->getNumUsers() <= 0 )
	{
		delete m_resources[ _name ];
		m_resources.erase( _name );
	}
}
