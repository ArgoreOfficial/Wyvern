#include "query.h"

#include <wv/runtime/properties.h>
#include <wv/runtime/callable.h>

void wv::IRuntimeQuery::dump()
{
	wv::Debug::Print( "# %s:%s\n", name.c_str(), base.c_str() );

	wv::Debug::Print( ": Methods:\n" );
	for ( auto& m : pFunctions->list() )
		wv::Debug::Print( ":   %s\n", m.c_str() );
	
	wv::Debug::Print( ": Properties:\n" );
	for ( auto& p : pProperties->list() )
		wv::Debug::Print( ":   %s\n", p.c_str() );
}
