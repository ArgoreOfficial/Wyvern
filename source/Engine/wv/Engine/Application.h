#pragma once

#include <wv/Decl.h>

namespace wv
{
	class iApplication
	{
	WV_DECLARE_INTERFACE( iApplication )

	public:
		virtual bool create ( void ) = 0;
		virtual void run    ( void ) = 0;
		virtual void destroy( void ) = 0;
	};
}