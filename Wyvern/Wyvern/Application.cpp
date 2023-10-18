#include "Application.h"
#include <Wyvern/Game.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Managers/AssetManager.h>
#include <thread>

using namespace WV;

void WV::Application::windowResizeCallback( GLFWwindow* _window, int _width, int _height )
{
	getInstance().m_window->windowResizeCallback( _window, _width, _height );
}

void Application::init( Game* _game )
{
	int major = 0;
	int minor = 1;

	WVINFO( "Loading Wyvern runtime version %i.%i", major, minor );

	if ( _game == nullptr )
	{
		WVFATAL( "Game instance failed to create!" );
		return;
	}

	m_game = _game;
	WVDEBUG( "Game instance created" );

	m_window = new WV::Window();
	if ( !m_window->createWindow( "Wyvern" ) )
	{
		delete m_window;
		return;
	}

	glfwSetFramebufferSizeCallback( m_window->getWindow(), Application::windowResizeCallback );
}

void Application::deinit()
{

}

void WV::Application::initImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	WVDEBUG( "ImGui Context Created" );
	m_io = &ImGui::GetIO();
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	// imgui-glfw backend specific init
	bgfx::RendererType::Enum backend = bgfx::getRendererType();
	switch ( backend )
	{
	case( bgfx::RendererType::Vulkan ):
		ImGui_ImplGlfw_InitForVulkan( m_window->getWindow(), true );
		WVTRACE( "ImGui_ImplGlfw_InitForVulkan" );
		break;

	case( bgfx::RendererType::OpenGL ):
		ImGui_ImplGlfw_InitForOpenGL( m_window->getWindow(), true );
		WVTRACE( "ImGui_ImplGlfw_InitForOpenGL" );
		break;

	default:
		ImGui_ImplGlfw_InitForOther( m_window->getWindow(), true );
		WVTRACE( "ImGui_ImplGlfw_InitForOther" );
		break;
	}

	int viewid = 255;
	ImGui_Implbgfx_Init( viewid );
	WVTRACE( "ImGui_Implbgfx_Init(%i)", viewid );
}

void WV::Application::styleImgui()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowMinSize = ImVec2( 160, 20 );
	style.FramePadding = ImVec2( 4, 2 );
	style.ItemSpacing = ImVec2( 6, 2 );
	style.ItemInnerSpacing = ImVec2( 6, 4 );
	style.Alpha = 0.95f;
	style.WindowRounding = 4.0f;
	style.FrameRounding = 2.0f;
	style.IndentSpacing = 6.0f;
	style.ItemInnerSpacing = ImVec2( 2, 4 );
	style.ColumnsMinSpacing = 50.0f;
	style.GrabMinSize = 14.0f;
	style.GrabRounding = 16.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 16.0f;

	style.Colors[ ImGuiCol_Text ] = ImVec4( 0.86f, 0.93f, 0.89f, 0.78f );
	style.Colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.86f, 0.93f, 0.89f, 0.28f );
	style.Colors[ ImGuiCol_WindowBg ] = ImVec4( 0.13f, 0.14f, 0.17f, 1.00f );
	style.Colors[ ImGuiCol_Border ] = ImVec4( 0.31f, 0.31f, 1.00f, 0.00f );
	style.Colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	style.Colors[ ImGuiCol_FrameBg ] = ImVec4( 0.20f, 0.22f, 0.27f, 1.00f );
	style.Colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.92f, 0.18f, 0.29f, 0.78f );
	style.Colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_TitleBg ] = ImVec4( 0.20f, 0.22f, 0.27f, 1.00f );
	style.Colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.20f, 0.22f, 0.27f, 0.75f );
	style.Colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.20f, 0.22f, 0.27f, 0.47f );
	style.Colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.20f, 0.22f, 0.27f, 1.00f );
	style.Colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.09f, 0.15f, 0.16f, 1.00f );
	style.Colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.92f, 0.18f, 0.29f, 0.78f );
	style.Colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_CheckMark ] = ImVec4( 0.71f, 0.22f, 0.27f, 1.00f );
	style.Colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.47f, 0.77f, 0.83f, 0.14f );
	style.Colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_Button ] = ImVec4( 0.47f, 0.77f, 0.83f, 0.14f );
	style.Colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.92f, 0.18f, 0.29f, 0.86f );
	style.Colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_Header ] = ImVec4( 0.92f, 0.18f, 0.29f, 0.76f );
	style.Colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.92f, 0.18f, 0.29f, 0.86f );
	style.Colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_Separator ] = ImVec4( 0.14f, 0.16f, 0.19f, 1.00f );
	style.Colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.92f, 0.18f, 0.29f, 0.78f );
	style.Colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.47f, 0.77f, 0.83f, 0.04f );
	style.Colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.92f, 0.18f, 0.29f, 0.78f );
	style.Colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_PlotLines ] = ImVec4( 0.86f, 0.93f, 0.89f, 0.63f );
	style.Colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.86f, 0.93f, 0.89f, 0.63f );
	style.Colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 0.92f, 0.18f, 0.29f, 1.00f );
	style.Colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.92f, 0.18f, 0.29f, 0.43f );
	style.Colors[ ImGuiCol_PopupBg ] = ImVec4( 0.20f, 0.22f, 0.27f, 0.9f );
	style.Colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.20f, 0.22f, 0.27f, 0.73f );
}


void Application::internalRun( Game* _game )
{
	init( _game );
	initImgui();
	styleImgui();

	m_game = getInstance().m_game;
	m_window = getInstance().m_window;

	m_game->load();
	std::thread* loadthread = cAssetManager::loadQueuedAssets();

	double loadtimer = glfwGetTime();
	while ( cAssetManager::isLoading() )
	{
		m_window->touch();

		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf( 0, 0, 0x0f, "Wyvern Engine Loading..." );
		bgfx::setDebug( BGFX_DEBUG_TEXT );

		bgfx::frame();
	}

	loadthread->join();
	m_game->start();

	WVDEBUG( "Loading took %.5f seconds", ( glfwGetTime() - loadtimer ) );

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
	bgfx::shutdown();
	m_window->shutdown();
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

	if ( m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent( backup_current_context );
	}

	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf( 0, 0, 0x0f, "Wyvern Engine Debug" );
	bgfx::dbgTextPrintf( 0, 1, 0x0f, "FPS: %f", 1.0f / m_deltaTime );
	bgfx::setDebug( BGFX_DEBUG_TEXT );


	bgfx::frame();
}
