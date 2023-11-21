#include "cApplication.h"
#include <Wyvern/Core/iLayer.h>
#include <Wyvern/Core/cLayerStack.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Managers/cAssetManager.h>

#include <thread>

using namespace wv;

void cApplication::run()
{

	cApplication& instance = getInstance();

	if ( !instance.create() )
	{
		WV_FATAL( "Application could not be created!" );
		return;
	}
	instance.initImgui();

	instance.internalRun();
	instance.shutdown();
}

void wv::cApplication::startLoadThread()
{
	std::thread* loadthread = cAssetManager::loadQueuedAssets();
	double loadtimer = glfwGetTime();

	while ( cAssetManager::isLoading() )
	{
		m_viewport.clear();

		// draw

		m_viewport.clear();
	}
	loadthread->join();
	WV_DEBUG( "Loading took %.5f seconds", ( glfwGetTime() - loadtimer ) );
}

int cApplication::create()
{
	int major = 0;
	int minor = 2;

	WV_INFO( "Loading Wyvern runtime version %i.%i", major, minor );

	m_viewport.create( "Wyvern", 500, 500 );

	return 1;
}

void cApplication::destroy()
{

}

void wv::cApplication::initImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	WV_DEBUG( "ImGui Context Created" );

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enavbe Viewports
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		style.WindowRounding = 0.0f;
		style.Colors[ ImGuiCol_WindowBg ].w = 1.0f;
	}

	// should prbably be moved out somewhere else
	m_viewport.initImguiImpl();
	ImGui_ImplOpenGL3_Init();
	WV_DEBUG( "ImGui_ImplOpenGL3_Init()" );
}

void cApplication::internalRun()
{
	m_layerStack.start();
	startLoadThread();

	m_lastTime = 0.0;
	bool run = true;
	while ( run )
	{
		update();
		draw();

		if ( m_viewport.shouldClose() ) { run = false; }
	}
}

void cApplication::update()
{
	m_time = glfwGetTime();
	m_deltaTime = m_time - m_lastTime;
	m_lastTime = m_time;

	std::string title = "Wyvern " + std::to_string( 1.0f / m_deltaTime );
	m_viewport.setTitle(title.c_str());

	m_viewport.update();

	m_layerStack.update( m_deltaTime );
}

void cApplication::draw()
{

	ImGuiIO& io = ImGui::GetIO();;
	
	m_layerStack.draw3D();

	m_layerStack.draw2D();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_layerStack.drawUI();

	// move most of this out somewhere
	ImGui::Render();
	int width = m_viewport.getWidth();
	int height = m_viewport.getHeight();

	glViewport( 0, 0, width, height );
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent( backup_current_context );
	}

	m_viewport.display();

}

void cApplication::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown(); // move to viewport
	ImGui::DestroyContext();
	m_viewport.destroy();
}
