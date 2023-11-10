#pragma once
#include "iEvent.h"

namespace WV
{
	namespace Events
	{
		class MouseEvent : public iEvent<MouseEvent>
		{
		protected:
			MouseEvent() { }
		};
	}
}
