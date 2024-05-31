#include "Application.h"

#include <stdio.h>

#include <wv/Device/GraphicsDevice.h>
#include <glad/glad.h>
#include <wv/Device/Context.h>


#include <wv/Assets/Texture.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <math.h>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

wv::Application::Application( ApplicationDesc* _desc )
{
	wv::ContextDesc ctxDesc;
	ctxDesc.name = _desc->title;
	ctxDesc.width = _desc->windowWidth;
	ctxDesc.height = _desc->windowHeight;

#ifdef EMSCRIPTEN
	/// force to OpenGL ES 2 3.0
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL_ES2;
	ctxDesc.graphicsApiVersion.major = 3;
	ctxDesc.graphicsApiVersion.minor = 0;
#else
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL;
	ctxDesc.graphicsApiVersion.major = 4;
	ctxDesc.graphicsApiVersion.minor = 6;
#endif
	context = new wv::Context( &ctxDesc );

	wv::GraphicsDeviceDesc deviceDesc;
	deviceDesc.loadProc = context->getLoadProc();
	deviceDesc.graphicsApi = ctxDesc.graphicsApi; // must be same as context

	device = wv::GraphicsDevice::createGraphicsDevice( &deviceDesc );

	m_defaultRenderTarget = new RenderTarget();
	m_defaultRenderTarget->width = _desc->windowWidth;
	m_defaultRenderTarget->height = _desc->windowHeight;
	m_defaultRenderTarget->fbHandle = 0;
	
	device->setRenderTarget( m_defaultRenderTarget );
	
	s_instance = this;





	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL( context->m_windowContext, true );
#ifdef EMSCRIPTEN
	ImGui_ImplOpenGL3_Init( "#version 100" );
#else
	ImGui_ImplOpenGL3_Init( "#version 460" );
#endif

}

wv::Application* wv::Application::getApplication()
{
	return s_instance;
}

#ifdef EMSCRIPTEN
void emscriptenMainLoop() { wv::Application::getApplication()->tick(); }
#endif


/// TEMPORARY---
// called the first time device->draw() is called that frame
void pipelineCB( wv::UniformBlockMap& _uniformBlocks )
{
	wv::Application* app = wv::Application::getApplication();
	wv::Context* ctx = app->context;

	// material properties
	wv::UniformBlock& fragBlock = _uniformBlocks[ "UbInput" ];
	const wv::float3 psqYellow = { 0.9921568627f, 0.8156862745f, 0.03921568627f };

	fragBlock.set<wv::float3>( "u_Color", psqYellow );
	fragBlock.set<float>( "u_Alpha", 1.0f );

	// camera transorm
	wv::UniformBlock& block = _uniformBlocks[ "UbInstanceData" ];

	glm::mat4x4 projection = app->currentCamera->getProjectionMatrix();
	glm::mat4x4 view = app->currentCamera->getViewMatrix();
	
	block.set( "u_Projection", projection );
	block.set( "u_View", view );

	// bind texture to slot 0
	app->device->bindTextureToSlot( app->m_texture, 0 );
}

// called every time device->draw() is called 
void instanceCB( wv::UniformBlockMap& _uniformBlocks )
{
	wv::Application* app = wv::Application::getApplication();
	wv::Context* ctx = app->context;

	// model transform
	wv::UniformBlock& instanceBlock = _uniformBlocks[ "UbInstanceData" ];

	glm::mat4x4 model{ 1.0f };
	float t0 = sin( app->m_throb )        * 0.2f + 1.0f;
	float t1 = sin( app->m_throb + 1.3f ) * 0.2f + 1.0f;
	float t2 = sin( app->m_throb + 0.3f ) * 0.2f + 1.0f;
	model = glm::scale( model, { t0,t1,t2 } );
	instanceBlock.set( "u_Model", model );
}
/// ---TEMPORARY


void wv::Application::run()
{
	/// TODO: not this
	{
		wv::ShaderSource shaders[] = {
			{ wv::WV_SHADER_TYPE_VERTEX,   "res/vert.glsl" },
			{ wv::WV_SHADER_TYPE_FRAGMENT, "res/frag.glsl" }
		};

		/// TODO: change to UniformDesc?
		const char* ubInputUniforms[] = {
			"u_Color",
			"u_Alpha"
		};

		const char* ubInstanceDataUniforms[] = {
			"u_Projection",
			"u_View",
			"u_Model",
		};

		wv::UniformBlockDesc uniformBlocks[] = {
			{ "UbInput",        ubInputUniforms,        2 },
			{ "UbInstanceData", ubInstanceDataUniforms, 3 }
		};

		wv::PipelineDesc pipelineDesc;
		pipelineDesc.type = wv::WV_PIPELINE_GRAPHICS;
		pipelineDesc.topology = wv::WV_PIPELINE_TOPOLOGY_TRIANGLES;
		pipelineDesc.layout; /// TODO: fix
		pipelineDesc.shaders = shaders;
		pipelineDesc.uniformBlocks = uniformBlocks;
		pipelineDesc.numUniformBlocks = 2;
		pipelineDesc.numShaders = 2;
		pipelineDesc.instanceCallback = instanceCB;
		pipelineDesc.pipelineCallback = pipelineCB;

		m_pipeline = device->createPipeline( &pipelineDesc );
	}


	{
		wv::InputLayoutElement elements[] = {
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 },
			{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }
		};

		wv::InputLayout layout;
		layout.elements = elements;
		layout.numElements = 2;

		/*
		// data
		int pnumIndices = 0;
		int pnumVertices = 36;

		std::ofstream cubefile( "res/cube.wpr" );
		// header
		cubefile.write( (char*)&pnumIndices, sizeof( int ) );
		cubefile.write( (char*)&pnumVertices, sizeof( int ) );
		// data
		cubefile.write( (char*)indices, sizeof( indices ) );
		cubefile.write( (char*)skyboxVertices, sizeof( skyboxVertices ) );
		cubefile.close();
		*/
		
		/// TODO: change to proper asset loader
		std::ifstream in( "res/cube.wpr", std::ios::binary );
		std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

		int numIndices = *reinterpret_cast<int*>( buf.data() );
		int numVertices = *reinterpret_cast<int*>( buf.data() + sizeof( int ) );
		int vertsSize = numVertices * sizeof( float ) * 5; // 5 floats per vertex
		int indsSize = numIndices * sizeof( unsigned int );

		char* indexBuffer = buf.data() + ( sizeof( int ) * 2 );
		char* vertexBuffer = indexBuffer + indsSize;

		printf( "numVertices :%i\n", numVertices );
		printf( "numIndices  :%i\n", numIndices );
		printf( "vertsSize   :%i\n", vertsSize );
		printf( "indsSize    :%i\n", indsSize );

		wv::PrimitiveDesc prDesc;
		{
			prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
			prDesc.layout = &layout;

			prDesc.vertexBuffer = vertexBuffer;
			prDesc.vertexBufferSize = vertsSize;
			prDesc.numVertices = numVertices; /// TODO: don't hardcode

			prDesc.indexBuffer = 0;
			prDesc.indexBufferSize = 0;
			prDesc.numIndices = 0;
		}

		m_primitive = device->createPrimitive( &prDesc );
	}

	// Subscribe to mouse event
	subscribeMouseEvents();
	subscribeInputEvent();

	// cameras have to be made after the event is subscribed to
	// to get the correct order
	/// TODO: event priority
	orbitCamera = new OrbitCamera( ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	freeflightCamera = new FreeflightCamera( ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	orbitCamera->onCreate();
	freeflightCamera->onCreate();

	currentCamera = orbitCamera;

	TextureDesc texDesc;
	texDesc.filepath = "res/throbber.gif";
	m_texture = device->createTexture( &texDesc );

#ifdef EMSCRIPTEN
	emscripten_set_main_loop( &emscriptenMainLoop, 0, 1 );
#else
	while ( context->isAlive() )
		tick();
#endif
}

void wv::Application::terminate()
{
	context->terminate();
	device->terminate();

	delete context;
	delete device;
}

void wv::Application::tick()
{
	///imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	const ImGuiIO& io = ImGui::GetIO();
	IMouseListener::setEnabled( !io.WantCaptureMouse || !ImGui::IsWindowFocused( ImGuiFocusedFlags_AnyWindow ) );
	///
	

	
	context->pollEvents();
	double dt = context->getDeltaTime();
	m_throb += dt * m_throb_magnitude * m_throb_magnitude;

	currentCamera->update( dt );

	device->setActivePipeline( m_pipeline );
	device->clearRenderTarget( wv::Colors::White );
	
	device->draw( m_primitive );


	
	///imgui
	ImGui::SetNextWindowSize( { 250,250 } );
	if ( ImGui::Begin( "Good Morning!", 0, ImGuiWindowFlags_NoResize ) )
	{
		ImGui::SliderFloat( "throbber", &m_throb_magnitude, 0.0f, 3.0f );
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
	///

	context->swapBuffers();
}

void wv::Application::onResize( int _width, int _height )
{
	context->onResize( _width, _height );
	device->onResize( _width, _height );

	// recreate render target
	m_defaultRenderTarget->width = _width;
	m_defaultRenderTarget->height = _height;
	device->setRenderTarget( m_defaultRenderTarget );
}

void wv::Application::onMouseEvent( MouseEvent _event )
{
	if ( _event.button != MouseEvent::WV_MOUSE_BUTTON_RIGHT )
		return;

	if ( !_event.buttondown )
		return;

	if( currentCamera == orbitCamera )
	{
		currentCamera = freeflightCamera;
		freeflightCamera->getTransform().position = orbitCamera->getTransform().position;
		freeflightCamera->getTransform().rotation = orbitCamera->getTransform().rotation;

		( (FreeflightCamera*)freeflightCamera )->resetVelocity();
	}
}

void wv::Application::onInputEvent( InputEvent _event )
{
	if ( !_event.repeat )
		if ( _event.key == GLFW_KEY_F )
			if ( currentCamera != orbitCamera )
				currentCamera = orbitCamera; // lol
}
