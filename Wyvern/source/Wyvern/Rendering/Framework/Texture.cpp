#include "Texture.h"
using namespace WV;

Texture::Texture( std::string path ) :
	m_renderID( 0 ), _filePath( path ), _localBuffer( nullptr ), _width( 0 ), _height( 0 ), _bpp(0)
{
	glGenTextures( 1, &m_renderID );
	glBindTexture( GL_TEXTURE_2D, m_renderID );

	stbi_set_flip_vertically_on_load( 1 );
	_localBuffer = stbi_load( path.c_str(), &_width, &_height, &_bpp, 4 );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _localBuffer );
	Unbind();

	if( _localBuffer )
		stbi_image_free( _localBuffer );
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
