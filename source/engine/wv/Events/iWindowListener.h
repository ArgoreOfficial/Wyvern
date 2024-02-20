#pragma once

#include <wv/Decl.h>

namespace wv
{
	class iWindowListener
	{
		WV_DECLARE_INTERFACE( iWindowListener )

	protected:

		inline void subscribeWindowEvents()
		{

		}

		virtual void onWindowEvent() = 0;

	};
}

