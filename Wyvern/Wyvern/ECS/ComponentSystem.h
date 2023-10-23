#pragma once
#include <Wyvern/ECS/IComponent.h>
#include <vector>

namespace WV
{
	class ComponentSystem
	{
	public:
		void addComponent( IComponent* _component ) { m_components.push_back( _component ); }
		void start();
		void update( float _deltaTime );
		void exit();
	private:
		std::vector<IComponent*> m_components;
	};
}