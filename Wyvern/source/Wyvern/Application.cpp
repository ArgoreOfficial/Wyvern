#include "Application.h"
#include <Wyvern/Managers/AssetManager.h>
#include <Wyvern/Game.h>
#include <Wyvern/Rendering/Renderer.h>
#include <Wyvern/Logging/Logging.h>

using namespace WV;

void Application::init( Game* _game )
{
	Application& instance = getInstance();
	int major = 0;
	int minor = 1;

	WVINFO( "Loading Wyvern runtime version %i.%i", major, minor );

	if ( _game == nullptr )
	{
		WVFATAL( "Game instance failed to create!" );
		return;
	}

	instance.m_game = _game;
	WVDEBUG( "Game instance created" );

	instance.m_window = new WV::Window();
	if ( !instance.m_window->createWindow() )
	{
		delete instance.m_window;
		return;
	}
}

void Application::deinit()
{

}

void Application::internalRun( Game* _game )
{
	Application::init( _game );

	Game* game = getInstance().m_game;
	Window* window = getInstance().m_window;

	double lastTime = 0.0;

	game->load();
	/*
	while ( !AssetManager::isDoneLoading() )
	{
		game->loadUpdate();
		game->loadDraw();
	}
	*/
	

	bool run = true;
	while ( run )
	{
		m_time = glfwGetTime();
		float deltaTime = m_time - lastTime;
		lastTime = m_time;

		window->processInput();
		glClearColor( 0.4f, 0.4f, 0.6f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		game->update( deltaTime );
		game->draw();

		glfwSwapBuffers( window->getWindow() );
		run = window->pollEvents();
	}

	AssetManager::unloadAll();
}
