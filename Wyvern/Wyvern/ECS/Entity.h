#pragma once

namespace WV
{
	class Entity
	{
	public:
		Entity() { }
		~Entity() { }

		template<class T>
		T* addComponent() { return new T() }
	
	private:

	};
}