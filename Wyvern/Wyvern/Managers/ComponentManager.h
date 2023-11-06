#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <Wyvern/ECS/ComponentSystem.h>
#include <vector>

namespace WV
{
	class ComponentManager : public ISingleton<ComponentManager>
	{
	public:
		
	private:
		std::vector<void*> m_systems;
	};
}