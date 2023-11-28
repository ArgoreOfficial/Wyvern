#include <Wyvern/Renderer/Framework/cTexture2D.h>
#include <Wyvern/Logging/cLogging.h>

#define STB_IMAGE_IMPLEMENTATION
#include <vendor/stb_image.h>

#include <glad/gl.h>

void wv::cTexture2D::load( void )
{
	glGenTextures( 1, &m_handle );
	glBindTexture( GL_TEXTURE_2D, m_handle );
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load( true );
	unsigned char* data = stbi_load( m_path.c_str(), &width, &height, &nrChannels, 0);

	if ( data )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	else
	{
		 WV_ERROR("Failed to load texture %s", m_path.c_str());
	}
	
	stbi_image_free( data );
}

void wv::cTexture2D::bind( void )
{
	glBindTexture( GL_TEXTURE_2D, m_handle );
}
