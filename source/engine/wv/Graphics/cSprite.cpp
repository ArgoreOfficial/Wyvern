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
	m_material = cContentManager::getInstance().loadMaterial( _material );

	// m_transform.setScale( { (float)texture->width, (float)texture->height, 1.0f } );
	// m_aspect = (float)texture->height / (float)texture->width;
}

void wv::cSprite::render()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	cm::cWindow* window = cApplication::getInstance().getWindow();

	iCamera* camera = cApplication::getInstance().m_current_camera;
	
	float w = (float)window->getWidth();
	float h = (float)window->getHeight();

	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 view = camera->getViewMatrix();

	cTransform transform = m_transform;
	transform.scale.y *= m_aspect;
	glm::mat4 model = transform.getMatrix();
	
	m_material->bind();

	cVector4f col{ 1.0f, 1.0f, 1.0f, 1.0f };

	m_material->shader->uniformBlockBegin();
	m_material->shader->uniformBlockBuffer( "uProj", glm::value_ptr(projection), 64);
	m_material->shader->uniformBlockBuffer( "uView", glm::value_ptr( view ), 64 );
	m_material->shader->uniformBlockBuffer( "uModel", glm::value_ptr( model ), 64 );
	m_material->shader->uniformBlockBuffer( "uColor", &col, 16 );
	m_material->shader->uniformBlockEnd();

	backend->bindVertexArray( m_quad->vertex_array );
	backend->drawElements( 6, cm::eDrawMode::DrawMode_Triangle );
	
	backend->bindVertexArray( 0 );
	m_material->unbind();
}
