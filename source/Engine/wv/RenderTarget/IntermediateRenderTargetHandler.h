#pragma once

#include <wv/Types.h>
#include <wv/Math/Vector2.h>

namespace wv
{
	class iGraphicsDevice;

	class iIntermediateRenderTargetHandler
	{
	public:
		iIntermediateRenderTargetHandler() {};
		
		virtual bool shouldRecreate( void ) = 0;
		virtual void create        ( wv::iGraphicsDevice* _pGraphics ) = 0;
		void         destroy       ( void );

		virtual void draw( iGraphicsDevice* _pGraphics ) = 0;

		RenderTargetID m_pRenderTarget{};

	};
}