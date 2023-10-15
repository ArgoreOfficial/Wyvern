#pragma once
#include <string>

namespace WV
{
	namespace Asset
	{
		enum class AssetType
		{
			MESH, TEXTURE, SHADER
		};
	}

	class IAsset
	{
	public:
		virtual bool load( std::string _path ) { return false; }

	protected:
		IAsset(): _path("") { }
		~IAsset() { }
		std::string _path;
	};
}