#include "TextureAsset.h"
#include <vendor/stb_image.h>

using namespace WV;

TextureAsset::~TextureAsset()
{
	if ( m_buffer )
		stbi_image_free( m_buffer );
	
	delete m_buffer;
}

bool TextureAsset::load( std::string _path )
{
	stbi_set_flip_vertically_on_load( 1 );
	m_buffer = stbi_load( _path.c_str(), &m_width, &m_height, &m_bpp, 4);

    return true;
}

Asset::TextureAssetData WV::TextureAsset::getTextureData()
{
	Asset::TextureAssetData data;
	
	data.bufferPtr = m_buffer;
	data.width = m_width;
	data.height = m_height;
	data.bpp = m_bpp;
	
	return data;
}
