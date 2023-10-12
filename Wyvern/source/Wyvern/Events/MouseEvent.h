#pragma once
#include "IEvent.h"

namespace WV
{
	namespace Events
	{
		class MouseEvent : public IEvent<MouseEvent>
		{
		protected:
			MouseEvent() { }
		};
	}
}
