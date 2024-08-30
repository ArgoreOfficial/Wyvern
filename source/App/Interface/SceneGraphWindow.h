#pragma once

#include <vector>

namespace wv
{
	class iGraphicsDevice;
	class iSceneObject;

	namespace Editor
	{
		class cSceneGraphWindow
		{
		public:
			 cSceneGraphWindow() {}
			~cSceneGraphWindow() {}

			void draw( wv::iGraphicsDevice* _pGraphicsDevice );

			void drawSceneObject( wv::iSceneObject* _obj );

			std::vector<wv::iSceneObject*> m_selectedObjects;
		};
	}
}