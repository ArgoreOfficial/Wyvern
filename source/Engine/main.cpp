#include <stdio.h>

#include <glad/glad.h>

#include <wv/Device/GraphicsDevice.h>
#include <wv/Device/Context.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>
#include <wv/Shader/UniformBlock.h>

#include <math.h>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void initalize( wv::Context** _ctxOut, wv::GraphicsDevice** _deviceOut )
{
	wv::ContextDesc ctxDesc;
	ctxDesc.name = "Wyvern Renderer";
	ctxDesc.width = 800;
	ctxDesc.height = 600;
	ctxDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL;
	ctxDesc.graphicsApiVersion.major = 4;
	ctxDesc.graphicsApiVersion.minor = 6;

	wv::Context* ctx = new wv::Context( &ctxDesc );

	wv::GraphicsDeviceDesc deviceDesc;
	deviceDesc.loadProc = ctx->getLoadProc();
	deviceDesc.graphicsApi = wv::WV_GRAPHICS_API_OPENGL; // must be same as context

	wv::GraphicsDevice* device = wv::GraphicsDevice::createGraphicsDevice( &deviceDesc );

	*_ctxOut = ctx;
	*_deviceOut = device;
}

/// TEMPORARY
wv::Context* ctx;
wv::GraphicsDevice* device;
wv::Primitive* primitive;
wv::Pipeline* pipeline;
/// TEMPORARY

void mainLoop() 
{
	ctx->pollEvents();

	const float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
	device->clearRenderTarget( clearColor );

	/// TODO: move to GraphicsDevice
	if ( pipeline->pipelineCallback )
		pipeline->pipelineCallback( pipeline->uniformBlocks );

	device->draw( primitive );

	ctx->swapBuffers();
}

float rot = 0.0f;

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
	glm::mat4x4 view      { 1.0f };
	
	projection = glm::perspectiveFov( 1.0472f /*60 deg*/, 800.0f, 600.0f, 0.1f, 100.0f);

	view = glm::translate( view, { 0.0f, 0.0f, -4.0f } );

	rot += 0.016f;

	view = glm::rotate( view, rot, { 0, 1, 0 } );

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

int main()
{
	// wv::Context* ctx;
	// wv::GraphicsDevice* device;
	initalize( &ctx, &device );

	wv::DummyRenderTarget target{ 0, 800, 600 }; // temporary object until actual render targets exist
	device->setRenderTarget( &target );
	
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

		pipeline = device->createPipeline( &pipelineDesc );
	}

	
	{
		wv::InputLayoutElement layoutPosition;
		layoutPosition.num = 3;
		layoutPosition.type = wv::WV_FLOAT;
		layoutPosition.normalized = false;
		layoutPosition.stride = sizeof( float ) * 3;

		wv::InputLayout layout;
		layout.elements = &layoutPosition;
		layout.numElements = 1;

		float vertices[] = {
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.0f,  0.5f, 0.0f
		};

		std::ifstream in( "./res/psq.wpr", std::ios::binary );
		std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

		wv::PrimitiveDesc prDesc;
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;
		prDesc.vertexBuffer = reinterpret_cast<void*>( buf.data() );
		prDesc.vertexBufferSize = static_cast<unsigned int>( buf.size() );
		// prDesc.vertexBuffer = vertices;
		// prDesc.vertexBufferSize = sizeof( vertices );
		prDesc.numVertices = 3 * 16;

		primitive = device->createPrimitive( &prDesc );
	}

	device->setActivePipeline( pipeline );

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(&mainLoop, 0, 1);
#else
	while ( ctx->isAlive() )
	{
		mainLoop();
	}
#endif

	ctx->terminate();
	device->terminate();

	delete ctx;
	delete device;
}