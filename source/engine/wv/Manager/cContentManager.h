#pragma once

#include <wv/Core/iSingleton.h>
#include <string>

namespace wv
{
	class cContentManager : public iSingleton<cContentManager>
	{
	public:

		void onCreate() override;
		std::string loadShaderSource( const std::string& _path );
	};
}