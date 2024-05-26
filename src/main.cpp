#include <stdio.h>

#include <wv/GraphicsDevice.h>
#include <wv/Context.h>

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

int main()
{
	wv::Context* ctx;
	wv::GraphicsDevice* device;
	initalize( &ctx, &device );

	wv::DummyRenderTarget target{ 0, 800, 600 }; // temporary object until actual render targets exist
	device->setRenderTarget( &target );

	while ( ctx->isAlive() )
	{
		ctx->beginFrame();

		const float clearColor[ 4 ] = { 0.8f, 0.8f, 0.8f, 1.0f };
		device->clearRenderTarget( clearColor );

		ctx->endFrame();
	}

	ctx->terminate();

	delete ctx;
	delete device;
}