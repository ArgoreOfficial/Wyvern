#pragma once

#include <wv/RenderTarget/IntermediateRenderTargetHandler.h>

class cIRTViewportHandler : public wv::iIntermediateRenderTargetHandler
{
public:
	 cIRTViewportHandler( void ) { }
	~cIRTViewportHandler( void ) { }

	bool shouldRecreate   ( void ) override;
	void create( wv::iGraphicsDevice* _pGraphics ) override;

	void draw( wv::iGraphicsDevice* _pGraphics ) override;

private:

	wv::Vector2i m_viewportSize{ 100, 100 };

};