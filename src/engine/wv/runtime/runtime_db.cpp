#include "runtime_db.h"

void wv::RuntimeDataBase::dump()
{
	for( auto& query : m_queries )
	{
		printf( "# %s:%s\n", query.second->name, query.second->base );
		printf( ": Methods:\n" );
		for( auto& m : query.second->pMethods->list() )
			printf( ":   %s\n", m.c_str() );
		printf( ": Properties:\n" );
		for( auto& p : query.second->pProperties->list() )
			printf( ":   %s\n", p.c_str() );
	}
}
