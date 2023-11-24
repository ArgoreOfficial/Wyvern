#include "cApplication.h"

#include <Wyvern/Core/iLayer.h>
#include <Wyvern/Core/cLayerStack.h>
#include <Wyvern/Logging/cLogging.h>
#include <Wyvern/Managers/cAssetManager.h>

#include <Wyvern/Renderer/Framework/cIndexBuffer.h>
#include <Wyvern/Renderer/Framework/cVertexArray.h>
#include <Wyvern/Renderer/Framework/cVertexBuffer.h>
#include <Wyvern/Renderer/Framework/cVertexBufferLayout.h>
#include <Wyvern/Renderer/Framework/cShaderProgram.h>
#include <Wyvern/Assets/cShaderSource.h>

#include <thread>

using namespace wv;

///////////////////////////////////////////////////////////////////////////////////////

void cApplication::run( void )
{

	cApplication& instance = getInstance();

	if ( !instance.create() )
	{
		WV_FATAL( "Application could not be created!" );
		return;
	}

	// if( imguiEnabled )
	instance.initImgui();

	instance.internalRun();
	
	instance.destroy();

}

///////////////////////////////////////////////////////////////////////////////////////

void cApplication::startLoadThread( void )
{

	std::thread* loadthread = cAssetManager::loadQueuedAssets();
	double loadtimer = glfwGetTime();

	while ( cAssetManager::isLoading() )
	{
		m_viewport.clear();

		// loading time draw

		m_viewport.display();
	}
	loadthread->join();
	WV_DEBUG( "Loading took %.5f seconds", ( glfwGetTime() - loadtimer ) );

}

///////////////////////////////////////////////////////////////////////////////////////

int cApplication::create( void )
{

	int major = 0;
	int minor = 3;

	WV_INFO( "Loading Wyvern runtime version %i.%i", major, minor );

	m_viewport.create( "Wyvern", 640, 480 );

	return 1;

}

///////////////////////////////////////////////////////////////////////////////////////

void cApplication::destroy( void )
{

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown(); // move to viewport
	ImGui::DestroyContext();
	m_viewport.destroy();

}

///////////////////////////////////////////////////////////////////////////////////////

void cApplication::initImgui( void )
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

///////////////////////////////////////////////////////////////////////////////////////

void cApplication::internalRun( void )
{

	m_layerStack.start();
	startLoadThread();

	m_lastTime = 0.0;
	bool run = true;
	
	
	// opengl stuff

	float vertices[] = { // xyz rgb
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f  // top left 
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};


	cVertexArray vertexArray;
	vertexArray.create();

	cIndexBuffer indexBuffer;
	indexBuffer.create();
	indexBuffer.bufferData( indices, sizeof( indices ) );

	cVertexBuffer vertexBuffer;
	vertexBuffer.create();
	vertexBuffer.bufferData( vertices, sizeof( vertices ) );

	cVertexBufferLayout layout;
	layout.push( WV_TYPE::WV_FLOAT, 3 );
	layout.push( WV_TYPE::WV_FLOAT, 3 );

	vertexArray.addLayout( layout );
	vertexArray.addLayout( layout );

	cShaderSource vertSource( "assets/shaders/vert.shader" );
	vertSource.load();

	cShaderSource fragSource( "assets/shaders/frag.shader" );
	fragSource.load();

	cShaderProgram shaderProgram;
	shaderProgram.create( vertSource, fragSource );

	// select draw mode
	// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // default


	while ( run )
	{
		update();

		shaderProgram.use();
		vertexArray.bind();
		indexBuffer.bind();// glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
		glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

		// mesh.render();

		draw();

		if ( m_viewport.getState() == cViewport::eViewportState::kClosing ) 
			run = false;
	}

}

///////////////////////////////////////////////////////////////////////////////////////

void cApplication::update( void )
{
	m_time = glfwGetTime();
	m_deltaTime = m_time - m_lastTime;
	m_lastTime = m_time;

	std::string title = "Wyvern " + std::to_string( 1.0f / m_deltaTime );
	m_viewport.setTitle(title.c_str());

	m_viewport.update();

	m_layerStack.update( m_deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void cApplication::draw( void )
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

