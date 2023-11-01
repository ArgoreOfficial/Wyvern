#include "Application.h"
#include <Wyvern/Core/ILayer.h>
#include <Wyvern/Core/LayerStack.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Managers/AssetManager.h>

#include <thread>

using namespace WV;

void WV::Application::windowResizeCallback( GLFWwindow* _window, int _width, int _height )
{
	Application& instance = getInstance();
	instance.m_window->windowResizeCallback( _window, _width, _height );
	if ( instance.m_activeCamera )
		instance.m_activeCamera->setAspect( instance.m_window->getAspect() );
}

void Application::run()
{
	Application& instance = getInstance();

	if ( !instance.create() )
	{
		WVFATAL( "Application could not be created!" );
		return;
	}
	instance.initImgui();

	instance.internalRun();
	instance.shutdown();
}

void WV::Application::startLoadThread()
{
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
	WVDEBUG( "Loading took %.5f seconds", ( glfwGetTime() - loadtimer ) );
}

int Application::create( )
{
	int major = 0;
	int minor = 1;

	WVINFO( "Loading Wyvern runtime version %i.%i", major, minor );

	m_window = new WV::Window();
	if ( !m_window->createWindow( 700, 500, "SPETS 2" ) )
	{
		delete m_window;
		return 0;
	}

	glfwSetFramebufferSizeCallback( m_window->getWindow(), Application::windowResizeCallback );
	return 1;
}

void Application::destroy()
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

void Application::internalRun()
{	
	m_layerStack.start();
	startLoadThread();
	
	m_lastTime = 0.0;
	bool run = true;
	while ( run )
	{
		update();
		draw();

		if ( !m_window->pollEvents() ) { run = false; }
	}
}

void Application::update()
{
	m_time = glfwGetTime();
	m_deltaTime = m_time - m_lastTime;
	m_lastTime = m_time;

	m_window->processInput();

	m_layerStack.update( m_deltaTime );
}

void Application::draw()
{
	m_window->touch();

	if ( m_activeCamera )
	{
		m_activeCamera->setAspect( m_window->getAspect() );
		m_activeCamera->submit();
	}

	m_layerStack.draw3D();
	m_layerStack.draw2D();

	ImGui_Implbgfx_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_layerStack.drawUI();

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
	/*
	bgfx::dbgTextPrintf( 0, 0, 0x0f, "Wyvern Engine" );
	bgfx::dbgTextPrintf( 0, 1, 0x0f, "FPS: %.0f", 1.0f / m_deltaTime );
	
	if ( m_activeCamera )
	{
		bgfx::dbgTextPrintf( 0, 1, 0x0f, "Camera Pos: %f, %f, %f", m_activeCamera->m_position.x, m_activeCamera->m_position.y, m_activeCamera->m_position.z );
		bgfx::dbgTextPrintf( 0, 2, 0x0f, "Camera Rot: %f, %f, %f", m_activeCamera->m_rotation.x, m_activeCamera->m_rotation.y, m_activeCamera->m_rotation.z );
	}
	*/

	bgfx::setDebug( BGFX_DEBUG_TEXT );
	bgfx::frame();
}

void Application::shutdown()
{
	ImGui_Implbgfx_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	bgfx::shutdown();
	m_window->shutdown();
}
