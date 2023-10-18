#pragma once
#include <string>

namespace WV
{
	enum eAssetType
	{
		AUTO = 0,
		RAW = 1,
		MESH = 2,
		TEXTURE = 3,
		SHADER_SOURCE = 4
	};
	
	struct sAsset
	{
		std::string path;
		eAssetType assetType;
		unsigned char* data;
		unsigned int size;
	};

	namespace Asset
	{
		eAssetType getAssetTypeFromFile( std::string _path );
		std::string assetTypeToString( eAssetType _assetType );
	}
}