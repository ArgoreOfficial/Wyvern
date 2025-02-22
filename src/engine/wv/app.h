#pragma once

#include <wv/decl.h>

namespace wv
{
	class Engine;

	class IApplication
	{
	WV_DECLARE_INTERFACE( IApplication )

	public:
		virtual bool create ( void ) = 0;
		virtual void run    ( void ) = 0;
		virtual void destroy( void ) = 0;

	protected:

		wv::Engine* m_pEngine = nullptr;
	};
}