#pragma once

#include <wv/Decl.h>

namespace wv 
{
	class iRenderPass
	{
	WV_DECLARE_INTERFACE( iRenderPass )

	public:
		virtual void execute() = 0;
		
	};
}