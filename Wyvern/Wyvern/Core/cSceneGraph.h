#pragma once
#include "Wyvern/Core/ISceneObject.h"
#include <vector>

namespace WV
{
	class cSceneGraph
	{
	public:
		cSceneGraph() { }
		~cSceneGraph() { }

		void add( ISceneObject* _object );
		void update( double _deltaTime );
		void draw3D();
		void draw2D();
		void drawUI();
	private:
		unsigned int m_internalIDCount = 1;
		std::vector<ISceneObject*> m_objects;
	};

}