#include "IntermediateRenderTargetHandler.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/RenderTarget/RenderTarget.h>

void wv::iIntermediateRenderTargetHandler::destroy()
{
	if( !m_renderTarget.isValid() )
		return;

	wv::cEngine::get()->graphics->destroyRenderTarget( m_renderTarget );
}
