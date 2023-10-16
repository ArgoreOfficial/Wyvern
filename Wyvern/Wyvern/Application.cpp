#include "Application.h"
#include <Wyvern/Game.h>
#include <Wyvern/Logging/Logging.h>

#include <bgfx/bgfx.h>

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

	m_game = getInstance().m_game;
	m_window = getInstance().m_window;
	
	m_game->start();

	m_lastTime = 0.0;
	bool run = true;
	while ( run )
	{
		update();
		draw();

		run = m_window->pollEvents();
	}

	m_window->shutdown();
	bgfx::shutdown();
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
	m_window->touch();
	m_game->draw();
	
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf( 0, 0, 0x0f, "Wyvern Engine Debug" );
	bgfx::setDebug( BGFX_DEBUG_TEXT );

	bgfx::frame();

}
