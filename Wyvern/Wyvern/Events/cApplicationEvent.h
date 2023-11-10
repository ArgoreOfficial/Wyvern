#pragma once
#include "iEvent.h"

namespace WV
{
	namespace Events
	{
		class ApplicationEvent : public iEvent<ApplicationEvent>
		{
		protected:
			ApplicationEvent() { }
		};
	}
}
