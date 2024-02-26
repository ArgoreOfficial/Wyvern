#include "cSprite.h"

#include <wv/Core/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <wv/Graphics/Primitives.h>
#include <wv/Core/cApplication.h>
#include <wv/Managers/cContentManager.h>

#include <cm/Core/cWindow.h>
#include <wv/Camera/iCamera.h>

#include <glm/gtc/type_ptr.hpp>
#include <wv/Math/Vector4.h>

wv::cSprite::cSprite( void ):
	m_quad{ nullptr },
	m_material{ nullptr },
	m_transform{}
{

}

wv::cSprite::~cSprite( void )
{
	delete m_quad;
	delete m_material;
}

void wv::cSprite::create( const std::string& _material )
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	
	m_quad = wv::Primitives::quad();
	m_material = cContentManager::getInstance().getMaterial( _material );

	// m_transform.setScale( { (float)texture->width, (float)texture->height, 1.0f } );
	// m_aspect = (float)texture->height / (float)texture->width;
}

void wv::cSprite::render()
{
	cRenderer&    renderer = cRenderer::getInstance();
	cm::iBackend* backend  = cRenderer::getInstance().getBackend();

	iCamera* camera = cApplication::getInstance().m_current_camera;

	float w = (float)renderer.getViewportWidth();
	float h = (float)renderer.getViewportHeight();

	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 view = camera->getViewMatrix();

	cTransform transform = m_transform;
	transform.scale.y *= m_aspect;
	glm::mat4 model = transform.getMatrix();
	
	m_material->bind();

	cVector4f col{ 1.0f, 1.0f, 1.0f, 1.0f };

	m_material->shader->ubBegin();
	m_material->shader->ubBufferData( "uProj", glm::value_ptr(projection), 64);
	m_material->shader->ubBufferData( "uView", glm::value_ptr( view ), 64 );
	m_material->shader->ubBufferData( "uModel", glm::value_ptr( model ), 64 );
	m_material->shader->ubBufferData( "uColor", &col, 16 );
	m_material->shader->ubEnd();

	backend->bindVertexArray( m_quad->vertex_array );
	backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );
	
	backend->bindVertexArray( 0 );
	m_material->unbind();
}
