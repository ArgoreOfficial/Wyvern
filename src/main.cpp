#include <stdio.h>

#include <glad/glad.h>

#include <wv/GraphicsDevice.h>
#include <wv/Context.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>

#include <math.h>
#include <fstream>
#include <vector>

void initalize( wv::Context** _ctxOut, wv::GraphicsDevice** _deviceOut )
{
	wv::ContextDesc ctxDesc;
	ctxDesc.name = "Wyvern Renderer";
	ctxDesc.width = 1200;
	ctxDesc.height = 960;
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

	ctx->swapBuffers();
}

// called the first time device->draw() is called that frame
void pipelineUniformCallback()
{

}

// called every time device->draw() is called that frame
void instanceUniformCallback()
{

}

int main()
{
	// wv::Context* ctx;
	// wv::GraphicsDevice* device;
	initalize( &ctx, &device );

	wv::DummyRenderTarget target{ 0, 1200, 960 }; // temporary object until actual render targets exist
	device->setRenderTarget( &target );

	
	{
		wv::PipelineDesc pipelineDesc;
		wv::ShaderSource shaders[] = {
		#ifdef EMSCRIPTEN
			{ wv::WV_SHADER_TYPE_VERTEX,   "../res/vert.glsl" },
			{ wv::WV_SHADER_TYPE_FRAGMENT, "../res/frag.glsl" }
		#else
			{ wv::WV_SHADER_TYPE_VERTEX,   "../res/vert.glsl" },
			{ wv::WV_SHADER_TYPE_FRAGMENT, "../res/frag.glsl" }
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

		std::ifstream in( "../res/psq.wpr", std::ios::binary );
		std::vector<char> buf{ std::istreambuf_iterator<char>( in ), {} };

		wv::PrimitiveDesc prDesc;
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;
		prDesc.vertexBuffer = reinterpret_cast<void*>( buf.data() );
		prDesc.vertexBufferSize = static_cast<unsigned int>( buf.size() );
		prDesc.numVertices = 3 * 16;

		primitive = device->createPrimitive( &prDesc );
	}

	device->setActivePipeline( pipeline );

	/// TEMPORARY
	GLuint ub = glGetUniformBlockIndex( pipeline->program, "UbInput" );
	GLint ubSize = 0;
	glGetActiveUniformBlockiv( pipeline->program, ub, GL_UNIFORM_BLOCK_DATA_SIZE, &ubSize );

	GLuint ubBuffer;
	glGenBuffers( 1, &ubBuffer );
	glBindBuffer( GL_UNIFORM_BUFFER, ubBuffer );
	glBufferData( GL_UNIFORM_BUFFER, ubSize, 0, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );

	glBindBufferBase( GL_UNIFORM_BUFFER, 0, ubBuffer );

	const char* uniforms[] = { 
		"u_Color",
		"u_Alpha"
	};
	GLuint indices[ 2 ] = { 0, 0 };
	GLint offsets[ 2 ] = { 0, 0 };
	
	glGetUniformIndices( pipeline->program, 2, uniforms, indices );
	glGetActiveUniformsiv( pipeline->program, 2, indices, GL_UNIFORM_OFFSET, offsets );

	
	glUniformBlockBinding( pipeline->program, ub, 0 );

	/// TODO: don't hardcode lol
	float pbuf[ 4 ];// = new char[ ubSize ];

	float col[ 3 ] = { 253.0f / 255.0f, 208.0f / 255.0f, 10.0f / 255.0f };
	float alpha = 1.0f;

	memcpy( reinterpret_cast<char*>( pbuf ) + offsets[ 0 ], col, sizeof( col ) );
	memcpy( reinterpret_cast<char*>( pbuf ) + offsets[ 1 ], &alpha, sizeof( alpha ) );
	
	//delete[] pbuf;

	glBufferData( GL_UNIFORM_BUFFER, ubSize, pbuf, GL_DYNAMIC_DRAW );

	float r = 0.0f;
	float time = glfwGetTime();
	/// TEMPORARY

#ifdef EMSCRIPTEN
    emscripten_set_main_loop(&mainLoop, 0, 1);
#else
	while ( ctx->isAlive() )
	{
		ctx->pollEvents();
		
		const float clearColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
		device->clearRenderTarget( clearColor );

		/// TEMPORARY
		time = glfwGetTime();
		//glUniform3f( u_ColLoc, 253.0f / 255.0f, 208.0f / 255.0f, 10.0f / 255.0f );
		/// TEMPORARY

		device->draw( primitive );

		ctx->swapBuffers();
	}
#endif

	ctx->terminate();

	/// TEMPORARY
	delete ctx;
	delete device;
	/// TEMPORARY
}