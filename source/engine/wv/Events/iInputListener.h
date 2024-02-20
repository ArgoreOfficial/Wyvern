#pragma once

#include <wv/Decl.h>

namespace wv
{
	class iInputListener
	{
		WV_DECLARE_INTERFACE( iInputListener )

	protected:

		inline void subscribeInputEvents()
		{

		}

		virtual void onInputEvent() = 0;

	};
}

