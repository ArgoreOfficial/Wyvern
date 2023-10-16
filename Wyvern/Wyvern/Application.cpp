#include "Application.h"
#include <Wyvern/Game.h>
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
<<<<<<< HEAD
	ImGuiIO& io = ImGui::GetIO();

	ImGui_Implbgfx_Init( instance.m_window->getView() );
	ImGui_ImplGlfw_InitForOther( instance.m_window->getWindow(), true );
=======

	WVDEBUG("ImGui Context Created");

	outputLog( FATAL, "test" );
	ImGuiIO& io = ImGui::GetIO();

	ImGui_Implbgfx_Init( instance.m_window->getView() );
	WVTRACE("ImGui_Implbgfx_Init");

	ImGui_ImplGlfw_InitForOther( instance.m_window->getWindow(), true );
	WVTRACE( "ImGui_ImplGlfw_InitForOther" );
>>>>>>> d198a8d (imgui implementation)
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

	ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

<<<<<<< HEAD

=======
>>>>>>> d198a8d (imgui implementation)
	m_window->shutdown();
	bgfx::shutdown();
	WVDEBUG("BGFX Terminated");
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

	ImGui_Implbgfx_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	

	m_game->draw();
	
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf( 0, 0, 0x0f, "Wyvern Engine Debug" );
	bgfx::dbgTextPrintf( 0, 1, 0x0f, "FPS: %f", 1.0f / m_deltaTime );
	bgfx::setDebug( BGFX_DEBUG_TEXT );

<<<<<<< HEAD
	
	ImGui_Implbgfx_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	ImGui::ShowDemoWindow(); // your drawing here
=======
>>>>>>> d198a8d (imgui implementation)
	ImGui::Render();
	ImGui_Implbgfx_RenderDrawLists( ImGui::GetDrawData() );


	bgfx::frame();

}
