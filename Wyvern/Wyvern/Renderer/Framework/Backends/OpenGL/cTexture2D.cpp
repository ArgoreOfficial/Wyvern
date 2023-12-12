#include <Wyvern/Renderer/Framework/cTexture2D.h>
#include <Wyvern/Logging/cLogging.h>

#define STB_IMAGE_IMPLEMENTATION
#include <vendor/stb_image.h>

#include <glad/gl.h>

void wv::cTexture2D::load( std::string _path )
{

	// load and generate the texture
	stbi_set_flip_vertically_on_load( true );
	m_data = stbi_load( _path.c_str(), &m_width, &m_height, &m_nrChannels, 0);

	WV_INFO( "Loaded texture %s", _path.c_str() );

}

void wv::cTexture2D::create( void )
{

	glGenTextures( 1, &m_handle );
	glBindTexture( GL_TEXTURE_2D, m_handle );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	if ( m_data )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_data );
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	else
	{
		WV_ERROR( "Failed to load texture" );
	}

	stbi_image_free( m_data );
	glBindTexture( GL_TEXTURE_2D, 0 );

}

void wv::cTexture2D::bind( void )
{

	glBindTexture( GL_TEXTURE_2D, m_handle );

}
