#pragma once

namespace WV
{
	class IComponent
	{
	public:
		virtual void start() { }
		virtual void update( float _deltaTime ) { }
		virtual void exit() { }

	protected:
		IComponent() { }
		~IComponent() { }

	};


}