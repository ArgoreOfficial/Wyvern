#pragma once

#include <wv/decl.h>

namespace wv
{
	class cEngine;

	class iApplication
	{
	WV_DECLARE_INTERFACE( iApplication )

	public:
		virtual bool create ( void ) = 0;
		virtual void run    ( void ) = 0;
		virtual void destroy( void ) = 0;

	protected:

		wv::cEngine* m_pEngine = nullptr;
	};
}