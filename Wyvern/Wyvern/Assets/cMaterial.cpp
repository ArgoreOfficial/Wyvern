#include "cMaterial.h"

void wv::cMaterial::loadTexture( std::string _path )
{
	
	m_texture = new cTexture2D( _path );

}

void wv::cMaterial::load()
{

	cShaderSource vertSource( "assets/shaders/vert.shader" );
	vertSource.load();

	cShaderSource fragSource( "assets/shaders/frag.shader" );
	fragSource.load();

	m_shader;
	m_shader.create( vertSource, fragSource );

	if ( m_texture )
		m_texture->load();

}

void wv::cMaterial::use()
{
	if ( m_texture )
		m_texture->bind();

	m_shader.use();
}
