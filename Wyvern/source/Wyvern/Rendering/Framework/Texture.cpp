#include "Texture.h"

using namespace WV;

Texture::Texture( Asset::TextureAssetData _data )
	: m_renderID( 0 ), m_width( m_width ), m_height( _data.height ), m_bpp( _data.bpp )
{
	glGenTextures( 1, &m_renderID );
	glBindTexture( GL_TEXTURE_2D, m_renderID );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, _data.width, _data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data.bufferPtr );
	Unbind();
}

Texture::~Texture()
{
	glDeleteTextures( 1, &m_renderID );
}

void Texture::Bind( unsigned int slot ) const
{
	glActiveTexture( GL_TEXTURE0 + slot );

	glBindTexture( GL_TEXTURE_2D, m_renderID );
}

void Texture::Unbind() const
{
	glBindTexture( GL_TEXTURE_2D, 0 );
}
