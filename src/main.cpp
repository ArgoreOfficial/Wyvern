#include <stdio.h>

#include <wv/GraphicsDevice.h>
#include <wv/Context.h>
#include <wv/Pipeline/Pipeline.h>
#include <wv/Primitive/Primitive.h>

void initalize( wv::Context** _ctxOut, wv::GraphicsDevice** _deviceOut )
{
	wv::ContextDesc ctxDesc;
	ctxDesc.name = "Wyvern Renderer";
	ctxDesc.width = 350;
	ctxDesc.height = 256;
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

int main()
{
	wv::Context* ctx;
	wv::GraphicsDevice* device;
	initalize( &ctx, &device );

	wv::DummyRenderTarget target{ 0, 350, 256 }; // temporary object until actual render targets exist
	device->setRenderTarget( &target );

	wv::Pipeline* pipeline;
	{
		wv::PipelineDesc pipelineDesc;
		wv::ShaderSource shaders[] = {
			{ wv::WV_SHADER_TYPE_VERTEX,   "../res/basic.vert" },
			{ wv::WV_SHADER_TYPE_FRAGMENT, "../res/basic.frag" }
		};

		pipelineDesc.type = wv::WV_PIPELINE_GRAPHICS;
		pipelineDesc.topology = wv::WV_PIPELINE_TOPOLOGY_TRIANGLES;
		pipelineDesc.shaders = shaders;
		pipelineDesc.numShaders = 2;
		pipeline = device->createPipeline( &pipelineDesc );
	}

	wv::Primitive* primitive;
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

		wv::PrimitiveDesc prDesc;
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;
		prDesc.vertexBuffer = vertices;
		prDesc.vertexBufferSize = sizeof( vertices );
		prDesc.numVertices = 3;

		primitive = device->createPrimitive( &prDesc );
	}

	device->setActivePipeline( pipeline );

	while ( ctx->isAlive() )
	{
		ctx->beginFrame();

		const float clearColor[ 4 ] = { 0.8f, 0.8f, 0.8f, 1.0f };
		device->clearRenderTarget( clearColor );

		device->draw( primitive );

		ctx->endFrame();
	}

	ctx->terminate();

	delete ctx;
	delete device;
}