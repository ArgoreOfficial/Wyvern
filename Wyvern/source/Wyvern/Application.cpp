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

	instance.m_window->hookEvents();
}

void Application::deinit()
{

}

void Application::internalRun( Game* _game )
{
	Application::init( _game );

	m_game = getInstance().m_game;
	m_window = getInstance().m_window;

	AssetManager::load<ShaderSource>( "shaders/default.shader" );
	m_game->load();
	std::thread* loadthread = AssetManager::loadQueuedAssets();

	while ( AssetManager::isLoading() )
	{
		m_window->processInput();
		glClearColor( 0.1f, 0.1f, 0.15f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glfwSwapBuffers( m_window->getWindow() );
		m_window->pollEvents();
	}

	loadthread->join();
	m_game->start();

	m_lastTime = 0.0;
	bool run = true;
	while ( run )
	{
		update();
		draw();

		run = m_window->pollEvents();
	}

	AssetManager::unloadAll();
}

void Application::update()
{
	m_time = glfwGetTime();
	m_deltaTime = m_time - m_lastTime;
	m_lastTime = m_time;

	m_window->processInput();

	m_game->update( m_deltaTime );
}

void Application::draw()
{
	glClearColor( 0.4f, 0.4f, 0.6f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	m_game->draw();
	glfwSwapBuffers( m_window->getWindow() );
}

void Application::pollEvents()
{
}
