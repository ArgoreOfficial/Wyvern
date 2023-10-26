#pragma once
#include <Wyvern/Core/ILayer.h>
#include <vector>

namespace WV
{
	class LayerStack
	{
	public:
		LayerStack() { }
		~LayerStack() { }

		void start();
		void update( double _deltaTime );
		void draw3D();
		void draw2D();
		void drawUI();

		void push_back( ILayer* _layer ) { m_layers.push_back( _layer ); }

	private:
		std::vector<ILayer*> m_layers;

	};
}