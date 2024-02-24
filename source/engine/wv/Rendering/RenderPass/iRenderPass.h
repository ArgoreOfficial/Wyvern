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
		
		void onResize( int _width, int _height )
		{
			if ( m_framebuffer )
				m_framebuffer->onResize();
		}

		void bind  ( void ) { m_framebuffer->bind(); }
		void unbind( void ) { m_framebuffer->unbind(); }

		cFramebuffer* getFramebuffer( void ) { return m_framebuffer; }

	protected:
		cFramebuffer* m_framebuffer = nullptr;
		cShader*      m_shader      = nullptr;
	};
}