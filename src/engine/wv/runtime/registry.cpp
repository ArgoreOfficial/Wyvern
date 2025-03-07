#include "registry.h"

void wv::RuntimeRegistry::dump()
{
	for( auto& query : m_queries )
	{
		printf( "# %s:%s\n", query.second->name, query.second->base );
		printf( ": Methods:\n" );
		for( auto& m : query.second->pFunctions->list() )
			printf( ":   %s\n", m.c_str() );
		printf( ": Properties:\n" );
		for( auto& p : query.second->pProperties->list() )
			printf( ":   %s\n", p.c_str() );
	}
}

wv::IRuntimeObject* wv::RuntimeRegistry::instantiate( const std::string& _objectName )
{
	if( m_queries.count( _objectName ) == 0 )
		return nullptr; // error

	return m_queries[ _objectName ]->fptrConstruct();
}
