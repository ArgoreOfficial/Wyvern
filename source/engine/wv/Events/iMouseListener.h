#pragma once

#include <wv/Decl.h>

namespace wv
{
	class iMouseListener
	{
		WV_DECLARE_INTERFACE( iMouseListener )

	protected:

		inline void subscribeMouseEvents()
		{

		}

		virtual void onMouseEvent() = 0;

	};
}
