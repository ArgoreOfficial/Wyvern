#include <stdio.h>

#include <glad/glad.h>

#include <wv/GraphicsDevice.h>
#include <wv/Context.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>

#include <math.h>

void initalize( wv::Context** _ctxOut, wv::GraphicsDevice** _deviceOut )
{
	wv::ContextDesc ctxDesc;
	ctxDesc.name = "Wyvern Renderer";
	ctxDesc.width = 630;
	ctxDesc.height = 630;
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

wv::Context* ctx;
wv::GraphicsDevice* device;
wv::Primitive* primitive;
wv::Pipeline* pipeline;

void mainLoop() 
{
	ctx->beginFrame();

	//const float clearColor[ 4 ] = { 0.8f, 0.8f, 0.8f, 1.0f };
	const float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
	device->clearRenderTarget( clearColor );

	/// TEMPORARY
	// time = glfwGetTime();
	//float r = sin( time * 3.0f ) * 0.5f + 0.5f;
	//float g = sin( time * 2.0f ) * 0.5f + 0.5f;
	//float b = sin( time * 5.0f ) * 0.5f + 0.5f;

	GLint u_ColLoc = glGetUniformLocation( pipeline->program, "u_Col" );
	glUniform3f( u_ColLoc, 253.0f / 255.0f, 208.0f / 255.0f, 10.0f / 255.0f );
	device->draw( primitive );
	/// TEMPORARY

	ctx->endFrame();
}

int main()
{
	// wv::Context* ctx;
	// wv::GraphicsDevice* device;
	initalize( &ctx, &device );

	wv::DummyRenderTarget target{ 0, 630, 630 }; // temporary object until actual render targets exist
	device->setRenderTarget( &target );

	
	{
		wv::PipelineDesc pipelineDesc;
		wv::ShaderSource shaders[] = {
		#ifdef EMSCRIPTEN
			{ wv::WV_SHADER_TYPE_VERTEX,   "../res/basic_gles2.vert" },
			{ wv::WV_SHADER_TYPE_FRAGMENT, "../res/basic_gles2.frag" }
		#else
			{ wv::WV_SHADER_TYPE_VERTEX,   "../res/basic.vert" },
			{ wv::WV_SHADER_TYPE_FRAGMENT, "../res/basic.frag" }
		#endif
		};

		pipelineDesc.type = wv::WV_PIPELINE_GRAPHICS;
		pipelineDesc.topology = wv::WV_PIPELINE_TOPOLOGY_TRIANGLES;
		pipelineDesc.shaders = shaders;
		pipelineDesc.numShaders = 2;
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

		//float vertices[] = {
		//		-0.5f, -0.5f, 0.0f,
		//		 0.5f, -0.5f, 0.0f,
		//		 0.0f,  0.5f, 0.0f
		//};

		float vertices[] = {
-0.486068, -0.436594, -0.000000,
-0.588932, -0.203174, -0.000000,
-0.410899, -0.293641, -0.000000,
-0.486068, -0.436594, -0.000000,
-0.834220, -0.634408, -0.000000,
-0.588932, -0.203174, -0.000000,
0.018752, -0.634408, -0.000000,
0.018752, -0.436594, -0.000000,
-0.486068, -0.436594, -0.000000,
0.270371, -0.436594, -0.000000,
0.018752, -0.634408, -0.000000,
0.018752, -0.436594, -0.000000,
0.493636, -0.433429, -0.000000,
0.324176, -0.636518, -0.000000,
0.324176, -0.433429, -0.000000,
0.493636, -0.433429, -0.000000,
0.324176, -0.636518, -0.000000,
0.841788, -0.633880, -0.000000,
0.493636, -0.433429, -0.000000,
0.721781, -0.433429, -0.000000,
0.841788, -0.633880, -0.000000,
0.327473, 0.252588, 0.000000,
0.493636, -0.433429, -0.000000,
0.721781, -0.433429, -0.000000,
0.153397, 0.158957, 0.000000,
0.327473, 0.252588, 0.000000,
0.493636, -0.433429, -0.000000,
0.001740, 0.407790, 0.000000,
0.128341, 0.580695, 0.000000,
0.128341, 0.202476, 0.000000,
0.001740, 0.811742, 0.000000,
0.001740, 0.407790, 0.000000,
0.128341, 0.580695, 0.000000,
0.001740, 0.811742, 0.000000,
-0.386296, 0.148881, 0.000000,
0.001740, 0.407790, 0.000000,
-0.386296, 0.148881, 0.000000,
0.001740, 0.407790, 0.000000,
-0.386296, -0.250805, -0.000000,
0.018752, -0.634408, -0.000000,
-0.486068, -0.436594, -0.000000,
-0.834220, -0.634408, -0.000000
		};

		wv::PrimitiveDesc prDesc;
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;
		prDesc.vertexBuffer = vertices;
		prDesc.vertexBufferSize = sizeof( vertices );
		prDesc.numVertices = 3 * 16;

		primitive = device->createPrimitive( &prDesc );
	}

	device->setActivePipeline( pipeline );

	/// TEMPORARY
	GLint u_ColLoc = glGetUniformLocation( pipeline->program, "u_Col" );
	float r = 0.0f;
	float time = glfwGetTime();
	/// TEMPORARY

#ifdef EMSCRIPTEN
    printf( "Emscripten Main Loop" );
    emscripten_set_main_loop(&mainLoop, 0, 1);
#else
	while ( ctx->isAlive() )
	{
		ctx->beginFrame();

		//const float clearColor[ 4 ] = { 0.8f, 0.8f, 0.8f, 1.0f };
		const float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
		device->clearRenderTarget( clearColor );

		/// TEMPORARY
		time = glfwGetTime();
		//float r = sin( time * 3.0f ) * 0.5f + 0.5f;
		//float g = sin( time * 2.0f ) * 0.5f + 0.5f;
		//float b = sin( time * 5.0f ) * 0.5f + 0.5f;

		glUniform3f( u_ColLoc, 253.0f / 255.0f, 208.0f / 255.0f, 10.0f / 255.0f );
		device->draw( primitive );
		/// TEMPORARY

		ctx->endFrame();
	}
#endif

	ctx->terminate();

	delete ctx;
	delete device;
}