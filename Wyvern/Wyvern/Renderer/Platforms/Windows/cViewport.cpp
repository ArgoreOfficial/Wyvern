#include <Wyvern/Renderer/cViewport.h>
#include <Wyvern/Managers/cEventManager.h>

using namespace wv;

cViewport::cViewport()
{

}

cViewport::~cViewport()
{

}

void onResize( GLFWwindow* _window, int _width, int _height )
{
	/*
	cApplication& instance = getInstance();
	instance.m_window.windowResizeCallback( _window, _width, _height );

	if ( instance.m_activeCamera )
		instance.m_activeCamera->setAspect( instance.m_window.getAspect() );
	*/
}

void handleApplicationEvents()
{

}

void handleKeyEvents( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	Events::cKeyEvent keyEvent( key );
	cEventManager::call<Events::cKeyEvent>( keyEvent );

	if ( action == GLFW_PRESS )
	{
		Events::cKeyDownEvent downEvent( key );
		cEventManager::call<Events::cKeyDownEvent>( downEvent );
	}
	else if ( action == GLFW_RELEASE )
	{
		Events::cKeyUpEvent upEvent( key );
		cEventManager::call<Events::cKeyUpEvent>( upEvent );
	}

}

void handleMouseEvents( GLFWwindow* window, double xpos, double ypos )
{

}

void cViewport::create( std::string _title, unsigned int _width, unsigned int _height )
{
	// ----------------------- glfw -------------------------- //

	WV_DEBUG( "Creating Window [%i, %i]", _width, _height );

	if ( !glfwInit() )
	{
		WV_FATAL( "GLFW could not initialize!" );
		glfwTerminate();
		return;
	}
	WV_DEBUG( "GLFW Initialized" );

	m_window = glfwCreateWindow( _width, _height, _title.c_str(), nullptr, nullptr);
	
	if ( !m_window )
	{
		WV_FATAL( "Failed to create GLFW window" );
		glfwTerminate();
		return;
	}
	WV_DEBUG( "GLFW Window created" );

	glfwSetFramebufferSizeCallback( m_window, onResize );
	glfwMakeContextCurrent( m_window );
	gladLoadGL( glfwGetProcAddress );

	hookEvents();
}

void cViewport::destroy()
{
	WV_DEBUG( "Terminated GLFW" );
	glfwTerminate();
}

void cViewport::initImguiImpl()
{
	ImGui_ImplGlfw_InitForOpenGL( m_window, true );
}

void cViewport::hookEvents()
{
	glfwSetKeyCallback( m_window, handleKeyEvents );
	// glfwSetCursorPosCallback()
}

void cViewport::setTitle( const std::string& _title )
{
	glfwSetWindowTitle( m_window, _title.c_str() );
}

void cViewport::setVSync( bool _value )
{
	glfwSwapInterval( _value ? 1 : 0 );
}

void cViewport::update()
{
	glfwGetFramebufferSize( m_window, &m_viewWidth, &m_viewHeight );

	if ( glfwWindowShouldClose( m_window ) )
	{
		m_shouldClose = true;
		WV_DEBUG( "Closing" );
	}

	glfwPollEvents();
}

void cViewport::clear( const cColor& _color )
{
	m_clearColor = _color;

	const float red   = (float)( _color.m_red )   / 255.0f;
	const float green = (float)( _color.m_green ) / 255.0f;
	const float blue  = (float)( _color.m_blue )  / 255.0f;
	const float alpha = (float)( _color.m_alpha ) / 255.0f;
	
	glClearColor( red, green, blue, alpha );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void cViewport::display()
{
	glfwSwapBuffers( m_window );
}

void cViewport::processInput()
{
	if ( glfwGetKey( m_window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) // escape exit
	{
		glfwSetWindowShouldClose( m_window, true );
	}
}
