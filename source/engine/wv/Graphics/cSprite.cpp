#include "cSprite.h"

#include <cm/Core/stb_image.h>
#include <cm/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <wv/Graphics/Primitives.h>
#include <wv/Manager/cContentManager.h>
#include <wv/cApplication.h>

#include <cm/Core/cWindow.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

wv::cSprite::cSprite( void )
{

}

wv::cSprite::~cSprite( void )
{

}

void wv::cSprite::create( const std::string& _texture_path  )
{
	wv::cContentManager& content_manager = wv::cContentManager::getInstance();
	cm::cRenderer* renderer = cApplication::getInstance().getRenderer();
	cm::iBackend* backend = renderer->getBackend();
	
	/* mesh */
	m_quad = wv::Primitives::quad();

	/* shader */
	const std::string& vertex_shader_path = "../res/shaders/sprite.vert";
	const std::string& fragment_shader_path = "../res/shaders/sprite.frag";

	std::string vert = content_manager.loadShaderSource( vertex_shader_path );
	std::string frag = content_manager.loadShaderSource( fragment_shader_path );

	cm::sShader vert_shader = renderer->createShader( vert.data(), cm::eShaderType::Shader_Vertex );
	cm::sShader frag_shader = renderer->createShader( frag.data(), cm::eShaderType::Shader_Fragment );

	m_shader_program = backend->createShaderProgram();
	backend->attachShader( m_shader_program, vert_shader );
	backend->attachShader( m_shader_program, frag_shader );
	backend->linkShaderProgram( m_shader_program );

	/* texture */

	m_texture = backend->createTexture();
	unsigned char* data = stbi_load( _texture_path.c_str(), &m_texture.width, &m_texture.height, &m_texture.num_channels, 0);
	backend->generateTexture( m_texture, data );
	stbi_image_free( data );

}

void wv::cSprite::render()
{
	cm::cRenderer* renderer = cApplication::getInstance().getRenderer();
	cm::iBackend* backend = renderer->getBackend();
	cm::cWindow* window = cApplication::getInstance().getWindow();

	int proj_loc  = backend->getUniformLocation( m_shader_program, "uProj" );
	int view_loc  = backend->getUniformLocation( m_shader_program, "uView" );
	int model_loc = backend->getUniformLocation( m_shader_program, "uModel" );
	int color_loc = backend->getUniformLocation( m_shader_program, "uColor" );

	float w = (float)window->getWidth();
	float h = (float)window->getHeight();

	glm::mat4 projection = glm::ortho( -w / 2.0f, w / 2.0f,
								       -h / 2.0f, h / 2.0f,
								      -1000.0f, 1000.0f );
	glm::mat4 view( 1.0f );

	glm::mat4 model( 1.0f );
	model = glm::scale( model, { m_texture.width, m_texture.height, 1.0f } );

	backend->setUniformMat4f( proj_loc, glm::value_ptr( projection ) );
	backend->setUniformMat4f( view_loc, glm::value_ptr( view ) );
	backend->setUniformMat4f( model_loc, glm::value_ptr( model ) );
	backend->setUniformVec4f( color_loc, { 0.0f, 1.0f, 0.0f, 1.0f } );

	backend->useShaderProgram( m_shader_program );

	backend->bindVertexArray( m_quad->vertex_array );
	backend->bindTexture2D( m_texture.handle );

	backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );
}
