#pragma once
#include "Wyvern/Core/ISceneObject.h"
#include <vector>

namespace WV
{
	class SceneGraph
	{
	public:
		SceneGraph() { }
		~SceneGraph() { }

		void add( ISceneObject* _object );

	private:
		unsigned int m_internalIDCount = 1;
		std::vector<ISceneObject*> m_objects;
	};
}