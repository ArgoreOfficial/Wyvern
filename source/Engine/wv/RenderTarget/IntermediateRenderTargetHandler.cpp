#include "IntermediateRenderTargetHandler.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/RenderTarget/RenderTarget.h>

void wv::iIntermediateRenderTargetHandler::destroy()
{
	if( !m_pRenderTarget )
		return;

	wv::cEngine::get()->graphics->destroyRenderTarget( &m_pRenderTarget );
}
