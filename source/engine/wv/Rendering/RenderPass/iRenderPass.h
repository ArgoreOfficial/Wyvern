#pragma once

#include <wv/Decl.h>
#include <wv/Rendering/cFramebuffer.h>

namespace wv 
{
	class iRenderPass
	{
	WV_DECLARE_INTERFACE( iRenderPass )

	public:
		virtual void onCreate ( void ) = 0;
		virtual void onDestroy( void ) = 0;
		virtual void execute  ( cFramebuffer* _input_buffer ) = 0;
		
		cFramebuffer* getFramebuffer( void ) { return m_framebuffer; }

	protected:
		cFramebuffer* m_framebuffer = nullptr;
		cShader*      m_shader      = nullptr;
	};
}