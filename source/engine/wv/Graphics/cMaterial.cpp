#include "cMaterial.h"

#include <wv/Core/cRenderer.h>
#include <cm/Backends/iBackend.h>
#include <wv/Managers/cContentManager.h>

#include <wv/Core/cApplication.h>

#include <cm/Core/cWindow.h>
#include <wv/Camera/iCamera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

wv::cMaterial::cMaterial()
{
}

wv::cMaterial::~cMaterial()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	backend->destroyShaderProgram( shader->shader_program_handle );

	
	for ( auto& texture : m_textures )
		backend->destroyTexture( *texture.second );
	
	m_textures.clear();

	delete shader;
}

void wv::cMaterial::bind()
{
	if ( !shader )
		return;

	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	backend->useShaderProgram( shader->shader_program_handle );

	int offset = 0;
	if ( m_textures.size() == 0 )
		return;

	for ( auto& texture : m_textures )
	{
		backend->setUniformInt( shader->getUniformLocation( texture.first ), offset );
		backend->setActiveTextureSlot( offset );
		backend->bindTexture2D( texture.second->handle );
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
		printf( "Texture already added\n" );
		return m_textures[ _name ];
	}
	
	cm::sTexture2D* texture = cContentManager::getInstance().getTexture( _path );
	if ( !texture )
		return nullptr;

	m_textures[ _name ] = texture;
	return m_textures[ _name ];
}

cm::sTexture2D* wv::cMaterial::getTexture( std::string _name )
{
	if ( !shader )
		return nullptr;

	if ( m_textures.count( _name ) )
		return m_textures[ _name ];

	return nullptr;
}
