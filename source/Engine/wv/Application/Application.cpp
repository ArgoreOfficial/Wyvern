#include "Application.h"

#include <stdio.h>

#include <wv/Device/GraphicsDevice.h>
#include <glad/glad.h>
#include <wv/Device/Context.h>


#include <wv/Assets/Texture.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>

#include <math.h>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <wv/Camera/FreeflightCamera.h>
#include <wv/Camera/OrbitCamera.h>

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

	wv::DummyRenderTarget target{ 0, _desc->windowWidth, _desc->windowHeight }; /// temporary object until actual render targets exist
	device->setRenderTarget( &target );

	s_instance = this;
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

}

// called every time device->draw() is called 
void instanceCB( wv::UniformBlockMap& _uniformBlocks )
{
	// model transform
	wv::UniformBlock& instanceBlock = _uniformBlocks[ "UbInstanceData" ];
	glm::mat4x4 model{ 1.0f };
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

		/// TODO: change to proper asset loader
		// std::ifstream in( "res/psq.wpr", std::ios::binary );
		// std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

		//  position               texcoord
		float vertices[] = {
			-0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
			-0.5f,  0.5f, 0.0f,    0.0f, 1.0f, 
			 0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f,    1.0f, 0.0f
		};

		unsigned int indices[] = {
			0,1,2,
			0,2,3
		};

		wv::PrimitiveDesc prDesc;
		{
			prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
			prDesc.layout = &layout;
			//prDesc.vertexBuffer = reinterpret_cast<void*>( buf.data() );
			//prDesc.vertexBufferSize = static_cast<unsigned int>( buf.size() );
			//prDesc.numVertices = 3 * 16; /// TODO: don't hardcode

			prDesc.vertexBuffer = vertices;
			prDesc.vertexBufferSize = sizeof( vertices );
			prDesc.numVertices = 4; /// TODO: don't hardcode
		
			prDesc.indexBuffer = indices;
			prDesc.indexBufferSize = sizeof( indices );
			prDesc.numIndices = 6;
		}

		m_primitive = device->createPrimitive( &prDesc );
	}

	device->setActivePipeline( m_pipeline );

	// Subscribe to mouse event
	subscribeMouseEvents();

	// cameras have to be made after the event is subscribed to
	// to get the correct order
	/// TODO: event priority
	orbitCamera = new OrbitCamera( ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	freeflightCamera = new FreeflightCamera( ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	orbitCamera->onCreate();
	freeflightCamera->onCreate();

	currentCamera = orbitCamera;

	TextureDesc texDesc;
	texDesc.filepath = "res/jonas.png";
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
	context->pollEvents();

	double dt = context->getDeltaTime();

	const float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
	device->clearRenderTarget( clearColor );

	glBindTexture( GL_TEXTURE_2D, m_texture->handle );

	currentCamera->update( dt );

	/// TODO: move to GraphicsDevice
	if ( m_pipeline->pipelineCallback )
		m_pipeline->pipelineCallback( m_pipeline->uniformBlocks );

	device->draw( m_primitive );

	context->swapBuffers();
}

void wv::Application::onResize( int _width, int _height )
{
	context->onResize( _width, _height );
	device->onResize( _width, _height );

	// recreate render target
	wv::DummyRenderTarget target{ 0, _width, _height }; /// temporary object until actual render targets exist
	device->setRenderTarget( &target );
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
	else
		currentCamera = orbitCamera;
}
