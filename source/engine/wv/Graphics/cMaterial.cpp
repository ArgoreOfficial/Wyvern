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
	cContentManager& content_manager = cContentManager::getInstance();

	for ( auto& texture : m_textures )
		content_manager.destroyTexture( texture.second );
	
	m_textures.clear();
	
	if ( shader )
		content_manager.destroyShader( shader );
	
}

void wv::cMaterial::bind()
{
	if ( !shader )
		return;

	cm::iBackend* backend = cRenderer::getInstance().getBackend();

	backend->useShaderProgram( shader->shader_program_handle );

	if ( m_textures.size() == 0 )
		return;

	for ( auto& texture : m_textures )
	{
		int binding = shader->getUniformBinding( texture.first );
		
		backend->setActiveTextureSlot( binding );
		backend->bindTexture2D( texture.second->handle );
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

cm::sTexture2D* wv::cMaterial::addTexture( const std::string& _name, std::string _path )
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

	texture->name = _name;

	m_textures[ _name ] = texture;
	return m_textures[ _name ];
}

cm::sTexture2D* wv::cMaterial::getTexture( const std::string& _name )
{
	if ( !shader )
		return nullptr;

	if ( m_textures.count( _name ) )
		return m_textures[ _name ];

	return nullptr;
}
