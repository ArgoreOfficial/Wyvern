#include "cSprite.h"

#include <wv/Core/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <wv/Graphics/Primitives.h>
#include <wv/Core/cApplication.h>
#include <wv/Manager/cContentManager.h>

#include <cm/Core/cWindow.h>
#include <wv/Camera/cCamera.h>

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
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	
	m_quad = wv::Primitives::quad();
	m_material = cContentManager::getInstance().loadMaterial( "res/materials/sprite" );

	// m_transform.setScale( { (float)texture->width, (float)texture->height, 1.0f } );
	// m_aspect = (float)texture->height / (float)texture->width;
}

void wv::cSprite::render()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	cm::cWindow* window = cApplication::getInstance().getWindow();

	cCamera* camera = cApplication::getInstance().m_current_camera;
	
	float w = (float)window->getWidth();
	float h = (float)window->getHeight();

	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 view = camera->getViewMatrix();

	cTransform transform = m_transform;
	transform.scale.y *= m_aspect;
	glm::mat4 model = transform.getMatrix();
	
	m_material->bind();

	m_material->shader->setMatrix( "uProj",  glm::value_ptr( projection ) );
	m_material->shader->setMatrix( "uView",  glm::value_ptr( view ) );
	m_material->shader->setMatrix( "uModel", glm::value_ptr( model ) );
	m_material->shader->setVec4f ( "uColor", { 1.0f, 1.0f, 1.0f, 1.0f } );

	backend->bindVertexArray( m_quad->vertex_array );
	backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );
	
	backend->bindVertexArray( 0 );
	m_material->unbind();
}
