#pragma once

#include <wv/Rendering/RenderPass/iRenderPass.h>


namespace wv
{
	class cDeferredLightRenderPass : public iRenderPass
	{
	public:
		void onCreate ( void ) override;
		void onDestroy( void ) override;
		
		void execute( cFramebuffer* _input_buffer ) override;

	private:
		
	};
}