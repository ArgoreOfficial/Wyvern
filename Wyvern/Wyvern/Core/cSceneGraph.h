#pragma once
#include "Wyvern/Core/iSceneObject.h"
#include <vector>

namespace wv
{
	class cSceneGraph
	{
	public:
		cSceneGraph() { }
		~cSceneGraph() { }

		void add( iSceneObject* _object );
		void update( double _deltaTime );
		void draw3D();
		void draw2D();
		void drawUI();
	private:
		unsigned int m_internalIDCount = 1;
		std::vector<iSceneObject*> m_objects;
	};

}