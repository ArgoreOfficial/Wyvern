#pragma once
#include "IEvent.h"

namespace WV
{
	namespace Events
	{
		class ApplicationEvent : public IEvent<ApplicationEvent>
		{
		protected:
			ApplicationEvent() { }
		};
	}
}
