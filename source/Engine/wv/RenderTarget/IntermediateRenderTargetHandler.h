#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Types.h>

#include <wv/Math/Vector2.h>

namespace wv
{
	class iLowLevelGraphics;

	class iIntermediateRenderTargetHandler
	{
	public:
		iIntermediateRenderTargetHandler() {};
		
		virtual bool shouldRecreate( void ) = 0;
		virtual void create        ( wv::iLowLevelGraphics* _pGraphics ) = 0;
		void         destroy       ( void );

		virtual void draw( iLowLevelGraphics* _pGraphics ) = 0;

		RenderTargetID m_renderTarget{};

	};
}