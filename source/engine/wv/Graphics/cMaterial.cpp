#include "cMaterial.h"

#include <wv/Core/cRenderer.h>
#include <cm/Backends/iBackend.h>
#include <wv/Manager/cContentManager.h>

#include <wv/Core/cApplication.h>

#include <cm/Core/cWindow.h>
#include <wv/Camera/cCamera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <sstream>

wv::cMaterial::cMaterial()
{
}

wv::cMaterial::~cMaterial()
{
}

void wv::cMaterial::bind()
{
	if ( !shader )
		return;

	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	backend->useShaderProgram( shader->shader_program_handle );

	int offset = 0;
	for ( auto& texture : m_textures )
	{
		backend->setUniformInt( shader->getUniformLocation( texture.first ), offset );
		backend->setActiveTextureSlot( offset );
		backend->bindTexture2D( texture.second.handle );
		offset++;
	}

}

void wv::cMaterial::unbind()
{
	if ( !shader )
		return;

	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	int offset = 0;
	for ( auto& texture : m_textures )
	{
		backend->setActiveTextureSlot( offset );
		backend->bindTexture2D( 0 );
		offset++;
	}

	backend->useShaderProgram( shader->shader_program_handle );
}

cm::sTexture2D* wv::cMaterial::addTexture( std::string _name, std::string _path )
{
	if ( !shader )
		return nullptr;

	if ( m_textures.count( _name ) )
	{
		printf( "Texture already exists\n" );
		return &m_textures[ _name ];
	}
	
	cm::sTexture2D texture = cContentManager::getInstance().loadTexture( _path );

	m_textures[ _name ] = texture;
	return &m_textures[ _name ];
}

cm::sTexture2D* wv::cMaterial::getTexture( std::string _name )
{
	if ( !shader )
		return nullptr;

	if ( m_textures.count( _name ) )
		return &m_textures[ _name ];

	return nullptr;
}
