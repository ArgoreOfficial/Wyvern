#pragma once
#include "IAsset.h"

namespace WV
{
	namespace Asset
	{
		struct TextureAssetData
		{
			unsigned char* bufferPtr;
			int width, height, bpp;
		};
	}

	class TextureAsset : public IAsset
	{
	public:
		TextureAsset() : m_buffer( nullptr ), m_width( 0 ), m_height( 0 ), m_bpp( 0 ) { }
		~TextureAsset();

		bool load( std::string _path ) override;
		Asset::TextureAssetData getTextureData();
	private:
		unsigned char* m_buffer;
		int m_width, m_height, m_bpp;
	};
}

