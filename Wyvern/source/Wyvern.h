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
		static Wyvern& getInstance();
		static void deinit();
	};
}
