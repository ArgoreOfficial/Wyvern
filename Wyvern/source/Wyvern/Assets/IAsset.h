#pragma once
#include <string>

namespace WV
{
	class IAsset
	{
	public:
		virtual bool load( std::string _path, std::string _name = "" ) { return false; }

	protected:
		IAsset(): _path("") { }
		~IAsset() { }
		std::string _path;
	};
}