#pragma once

namespace WV
{
	class Game
	{
	public:
		Game();
		~Game();

		virtual void load() { }
		virtual void start() { }
		virtual void update( float _deltaTime ) { }
		virtual void draw() { }

	private:
		bool m_running = true;
	};
}
