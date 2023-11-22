#pragma once

#include <Wyvern/Core/ISingleton.h>
#include <vector>

namespace wv
{
	class ComponentManager : public iSingleton<ComponentManager>
	{
	public:
		
	private:
		std::vector<void*> m_systems;
	};
}