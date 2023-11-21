#pragma once
#include <string>

namespace wv
{
	class iLayer
	{
	public:
		iLayer( std::string _debugName = "Layer" ) : 
			m_debugName(_debugName), m_enabled(true) { }
		~iLayer() { }

		virtual void start() { }
		virtual void update( double _deltaTime ) { }
		virtual void draw3D() { }
		virtual void draw2D() { }
		virtual void drawUI() { }

	private:
		std::string m_debugName;
		bool m_enabled;
	};
}
