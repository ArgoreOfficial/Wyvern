#include "cApplication.h"
#include <Wyvern/Core/iLayer.h>
#include <Wyvern/Core/cLayerStack.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Managers/cAssetManager.h>

#include <thread>

using namespace WV;

void WV::cApplication::windowResizeCallback( GLFWwindow* _window, int _width, int _height )
{
	cApplication& instance = getInstance();
	instance.m_window->windowResizeCallback( _window, _width, _height );
	
	if ( instance.m_activeCamera )
		instance.m_activeCamera->setAspect( instance.m_window->getAspect() );
}

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

void WV::cApplication::startLoadThread()
{
	std::thread* loadthread = cAssetManager::loadQueuedAssets();
	double loadtimer = glfwGetTime();
	
	while ( cAssetManager::isLoading() )
	{
		glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );
	}
	loadthread->join();
	WV_DEBUG( "Loading took %.5f seconds", ( glfwGetTime() - loadtimer ) );
}

int cApplication::create( )
{
	int major = 0;
	int minor = 2;

	WV_INFO( "Loading Wyvern runtime version %i.%i", major, minor );

	m_window = new WV::cWindow();
	if ( !m_window->createWindow( 700, 500, "Wyvern" ) )
	{
		delete m_window;
		return 0;
	}

	glfwSetFramebufferSizeCallback( m_window->getWindow(), cApplication::windowResizeCallback );
	return 1;
}

void cApplication::destroy()
{

}

void WV::cApplication::initImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	WV_DEBUG( "ImGui Context Created" );
	m_io = &ImGui::GetIO();
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking


	ImGui_ImplGlfw_InitForOpenGL( m_window->getWindow(), true );
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

		if ( !m_window->pollEvents() ) { run = false; }
	}
}

void cApplication::update()
{
	m_time = glfwGetTime();
	m_deltaTime = m_time - m_lastTime;
	m_lastTime = m_time;

	m_window->processInput();

	m_layerStack.update( m_deltaTime );
}

void cApplication::draw()
{
	int display_w, display_h;
	glfwGetFramebufferSize( m_window->getWindow(), &display_w, &display_h);
	glViewport( 0, 0, display_w, display_h );
	glClearColor( 0.4f, 0.4f, 0.6f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	if ( m_activeCamera )
	{
		m_activeCamera->setAspect( m_window->getAspect() );
		m_activeCamera->submit();
	}

	m_layerStack.draw3D();

	m_layerStack.draw2D();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_layerStack.drawUI();

	ImGui::Render();
	
	if ( m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent( backup_current_context );
	}


	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
	glfwSwapBuffers( m_window->getWindow() );
}

void cApplication::shutdown()
{

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	m_window->shutdown();
}
