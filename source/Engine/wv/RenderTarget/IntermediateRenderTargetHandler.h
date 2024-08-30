#pragma once

#include <wv/Math/Vector2.h>

namespace wv
{
	class RenderTarget;
	class iGraphicsDevice;

	class iIntermediateRenderTargetHandler
	{
	public:
		iIntermediateRenderTargetHandler() {};
		
		virtual bool shouldRecreate( void ) = 0;
		virtual void create        ( wv::iGraphicsDevice* _pGraphics ) = 0;
		void         destroy       ( void );

		virtual void draw( iGraphicsDevice* _pGraphics ) = 0;

		RenderTarget* m_pRenderTarget = nullptr;

	};
}