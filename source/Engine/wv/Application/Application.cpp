#include "Application.h"

#include <stdio.h>

#include <wv/Device/GraphicsDevice.h>
#include <wv/Device/Context.h>

#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>

#include <math.h>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static wv::Application* appInstance = nullptr;

/// TEMPORARY---
static int screenWidth = 0;
static int screenHeight = 0;
/// ---TEMPORARY

wv::Application::Application( ApplicationDesc* _desc )
{
	screenWidth = _desc->windowWidth;
	screenHeight = _desc->windowHeight;

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
	m_ctx = new wv::Context( &ctxDesc );

	wv::GraphicsDeviceDesc deviceDesc;
	deviceDesc.loadProc = m_ctx->getLoadProc();
	deviceDesc.graphicsApi = ctxDesc.graphicsApi; // must be same as context

	m_device = wv::GraphicsDevice::createGraphicsDevice( &deviceDesc );

	wv::DummyRenderTarget target{ 0, _desc->windowWidth, _desc->windowHeight }; /// temporary object until actual render targets exist
	m_device->setRenderTarget( &target );

	appInstance = this;
}

#ifdef EMSCRIPTEN
void emscriptenMainLoop() { appInstance->tick(); }
#endif


/// TEMPORARY---
// called the first time device->draw() is called that frame
void pipelineCB( wv::UniformBlockMap& _uniformBlocks )
{
	// material properties
	wv::UniformBlock& fragBlock = _uniformBlocks[ "UbInput" ];
	const wv::float3 psqYellow = { 0.9921568627f, 0.8156862745f, 0.03921568627f };

	fragBlock.set<wv::float3>( "u_Color", psqYellow );
	fragBlock.set<float>( "u_Alpha", 1.0f );

	// camera transorm
	wv::UniformBlock& block = _uniformBlocks[ "UbInstanceData" ];

	glm::mat4x4 projection{ 1.0f };
	glm::mat4x4 view{ 1.0f };

	projection = glm::perspectiveFov( 1.0472f /*60 deg*/, (float)screenWidth, (float)screenHeight, 0.1f, 100.0f);

	view = glm::translate( view, { 0.0f, 0.0f, -4.0f } );

	appInstance->m_rot += 0.016f;
	view = glm::rotate( view, appInstance->m_rot, { 0, 1, 0 } );

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

		m_pipeline = m_device->createPipeline( &pipelineDesc );
	}


	{
		wv::InputLayoutElement layoutPosition{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 };

		wv::InputLayout layout;
		layout.elements = &layoutPosition;
		layout.numElements = 1;

		/// TODO: change to proper asset loader
		std::ifstream in( "res/psq.wpr", std::ios::binary );
		std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

		wv::PrimitiveDesc prDesc;
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;
		prDesc.vertexBuffer = reinterpret_cast<void*>( buf.data() );
		prDesc.vertexBufferSize = static_cast<unsigned int>( buf.size() );
		prDesc.numVertices = 3 * 16; /// TODO: don't hardcode

		m_primitive = m_device->createPrimitive( &prDesc );
	}

	m_device->setActivePipeline( m_pipeline );

#ifdef EMSCRIPTEN
	emscripten_set_main_loop( &emscriptenMainLoop, 0, 1 );
#else
	while ( m_ctx->isAlive() )
		tick();
#endif

}

void wv::Application::terminate()
{
	m_ctx->terminate();
	m_device->terminate();

	delete m_ctx;
	delete m_device;
}

void wv::Application::tick()
{
	m_ctx->pollEvents();

	const float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_device->clearRenderTarget( clearColor );

	/// TODO: move to GraphicsDevice
	if ( m_pipeline->pipelineCallback )
		m_pipeline->pipelineCallback( m_pipeline->uniformBlocks );

	m_device->draw( m_primitive );

	m_ctx->swapBuffers();
}
