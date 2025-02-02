#include "IntermediateRenderTargetHandler.h"

#include <wv/Engine/Engine.h>
#include <wv/Graphics/Graphics.h>
#include <wv/Graphics/RenderTarget.h>

void wv::iIntermediateRenderTargetHandler::destroy()
{
	if( !m_renderTarget.is_valid() )
		return;

	wv::cEngine::get()->graphics->destroyRenderTarget( m_renderTarget );
}
