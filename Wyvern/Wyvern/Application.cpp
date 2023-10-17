#include "Application.h"
#include <Wyvern/Game.h>
#include <Wyvern/Logging/Logging.h>

using namespace WV;

void WV::Application::windowResizeCallback( GLFWwindow* _window, int _width, int _height )
{
	getInstance().m_window->windowResizeCallback( _window, _width, _height );
}

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
	if ( !instance.m_window->createWindow( 512, 512, "Wyvern" ) )
	{
		delete instance.m_window;
		return;
	}

	glfwSetFramebufferSizeCallback( instance.m_window->getWindow(), Application::windowResizeCallback );



	// --------------- ImGui --------------- //

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	WVDEBUG("ImGui Context Created");
	ImGuiIO& io = ImGui::GetIO();

	ImGui_Implbgfx_Init( 255 );
	WVTRACE("ImGui_Implbgfx_Init");

	// imgui-glfw backend specific init
	bgfx::RendererType::Enum backend = bgfx::getRendererType();
	switch ( backend )
	{
	case( bgfx::RendererType::Vulkan ):
		ImGui_ImplGlfw_InitForVulkan( instance.m_window->getWindow(), true );
		WVTRACE( "ImGui_ImplGlfw_InitForVulkan" );
		break;

	case( bgfx::RendererType::OpenGL ):
		ImGui_ImplGlfw_InitForOpenGL( instance.m_window->getWindow(), true );
		WVTRACE( "ImGui_ImplGlfw_InitForOpenGL" );
		break;

	default:
		ImGui_ImplGlfw_InitForOther( instance.m_window->getWindow(), true );
		WVTRACE( "ImGui_ImplGlfw_InitForOther" );
		break;
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

	ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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

	m_game->draw();
	
	ImGui_Implbgfx_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_game->drawUI();

	ImGui::Render();
	ImGui_Implbgfx_RenderDrawLists( ImGui::GetDrawData() );
	/*
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf( 0, 0, 0x0f, "Wyvern Engine Debug" );
	bgfx::dbgTextPrintf( 0, 1, 0x0f, "FPS: %f", 1.0f / m_deltaTime );
	bgfx::setDebug( BGFX_DEBUG_TEXT );

	*/
	bgfx::frame();
}
