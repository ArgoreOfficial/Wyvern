#include "Asset.h"
#include <map>
#include <vector>

using namespace WV;

eAssetType Asset::getAssetTypeFromFile( std::string _path )
{
	std::map<std::string, WV::eAssetType> types = {
		{ ".obj", eAssetType::MESH },
		{ ".png", eAssetType::TEXTURE },
		{ ".shader", eAssetType::SHADER_SOURCE }
	};

	if ( !types.count( _path ) ) return eAssetType::RAW;
	return types[ _path ];
}

std::string WV::Asset::assetTypeToString( eAssetType _assetType )
{
	static std::vector<std::string> assetTypes = { "AUTO", "Raw", "Mesh", "Texture", "Shader Source" };

	return assetTypes[ _assetType ];
}
