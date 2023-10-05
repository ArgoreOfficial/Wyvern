#pragma once
#include "Wyvern/API/Core.h"

namespace WV
{
	class WYVERN_API Wyvern
	{
	public:
		Wyvern();
		~Wyvern();

		static void init();
		static void getInstance();

	private:
		static Wyvern* m_instance;
	};
}
