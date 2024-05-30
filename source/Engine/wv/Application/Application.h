#pragma once

namespace wv
{
	class Context;
	class GraphicsDevice;
	class Primitive;
	class Pipeline;
	class ICamera;

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
		static Application* getApplication();

		void run();
		void terminate();
		void tick();

		void onResize( int _width, int _height );

		/// TEMPORARY---
		Primitive* m_primitive = nullptr;
		Pipeline* m_pipeline = nullptr;
		float m_rot = 0.0f;
		/// ---TEMPORARY

		Context* context       = nullptr;
		GraphicsDevice* device = nullptr;

		ICamera* currentCamera = nullptr;

	private:

		/*
		 * technically not a singleton but getting a reference 
		 * to the application can sometimes be very useful.
		 * 
		 * this will have to change in case multiple applications 
		 * are to be supported
		 */
		static inline Application* s_instance = nullptr; 
	};
}