#pragma once
#include "iEvent.h"

namespace wv
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
