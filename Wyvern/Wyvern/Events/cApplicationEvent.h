#pragma once
#include "iEvent.h"

namespace wv
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
