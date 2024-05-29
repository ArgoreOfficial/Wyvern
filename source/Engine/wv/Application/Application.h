#pragma once

namespace wv
{
	class Context;
	class GraphicsDevice;
	class Primitive;
	class Pipeline;

	struct ApplicationDesc
	{
		const char* title;
		
		bool vsync = true;
		bool fullscreen = false;
		int windowWidth = 800;
		int windowHeight = 600;

		bool showDebugConsole = true;
	};

	class Application
	{
	public:
		Application( ApplicationDesc* _desc );

		void run();
		void terminate();

		/// TEMPORARY---
		Primitive* m_primitive;
		Pipeline* m_pipeline;
		float m_rot = 0.0f;
		/// ---TEMPORARY

		void tick();
	private:

		Context* m_ctx;
		GraphicsDevice* m_device;

	};
}