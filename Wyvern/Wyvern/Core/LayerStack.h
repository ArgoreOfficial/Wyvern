#pragma once
#include <Wyvern/Core/ILayer.h>
#include <vector>

namespace WV
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void start();
		void update( double _deltaTime );
		void draw3D();
		void draw2D();
		void drawUI();

	private:
		std::vector<ILayer*> m_layers;

	};
}