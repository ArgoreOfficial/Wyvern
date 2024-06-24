#pragma once

#include <wv/Decl.h>

namespace wv
{
	class IWindowListener
	{
		WV_DECLARE_INTERFACE( IWindowListener )

	protected:

		inline void subscribeWindowEvents()
		{

		}

		virtual void onWindowEvent() = 0;

	};
}

