#pragma once

#include <wv/Core/iSingleton.h>
#include <string>

namespace wv
{
	class cContentManager : public iSingleton<cContentManager>
	{
	public:

		void create() override;
		std::string loadShaderSource( const std::string& _path );
	};
}