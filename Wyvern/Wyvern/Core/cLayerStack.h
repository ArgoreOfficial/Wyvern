#pragma once
#include <Wyvern/Core/ILayer.h>
#include <vector>

namespace WV
{
	class cLayerStack
	{
	public:
		cLayerStack() { }
		~cLayerStack() { }

		void start();
		void update( double _deltaTime );
		void draw3D();
		void draw2D();
		void drawUI();

		void push_back( iLayer* _layer ) { m_layers.push_back( _layer ); }

	private:
		std::vector<iLayer*> m_layers;

	};
}