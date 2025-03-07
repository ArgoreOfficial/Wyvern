#include "runtime_db.h"

void wv::RuntimeDataBase::dump()
{
	int i = 0;
	for( auto& query : m_queries )
	{
		printf( "%02i : %s:%s\n", (int)i, query.second->name, query.second->base );
		printf( "Methods:\n" );
		for( auto& m : query.second->pMethods->methods )
			printf( "   %s\n", m );
		printf( "Properties:\n" );
		for( auto& p : query.second->pProperties->list() )
			printf( "   %s\n", p.c_str() );
		i++;
	}
}
