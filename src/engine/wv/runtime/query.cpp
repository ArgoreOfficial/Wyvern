#include "query.h"

#include <wv/runtime/properties.h>
#include <wv/runtime/callable.h>

void wv::IRuntimeQuery::dump()
{
	printf( "# %s:%s\n", name.c_str(), base.c_str() );

	printf( ": Methods:\n" );
	for ( auto& m : pFunctions->list() )
		printf( ":   %s\n", m.c_str() );
	
	printf( ": Properties:\n" );
	for ( auto& p : pProperties->list() )
		printf( ":   %s\n", p.c_str() );
}
