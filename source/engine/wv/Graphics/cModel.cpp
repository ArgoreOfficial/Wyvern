#include "cModel.h"

#include <cm/Core/stb_image.h>
#include <wv/Core/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <wv/Graphics/Primitives.h>
#include <wv/Managers/cContentManager.h>
#include <wv/Core/cApplication.h>

#include <cm/Core/cWindow.h>
#include <wv/Camera/iCamera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


wv::cModel::~cModel( void )
{
	for ( int i = 0; i < meshes.size(); i++ )
		delete meshes[ i ];

	meshes.clear();
}

void wv::cModel::create( std::string _path )
{
	
}

void wv::cModel::render()
{
	cRenderer&    renderer = cRenderer::getInstance();
	cm::iBackend* backend  = cRenderer::getInstance().getBackend();
	
	iCamera* camera = cApplication::getInstance().m_current_camera;

	float w = (float)renderer.getViewportWidth();
	float h = (float)renderer.getViewportHeight();

	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 view = camera->getViewMatrix();

	glm::mat4 model = transform.getMatrix();

	for ( int i = 0; i < meshes.size(); i++ )
	{
		cMaterial& material = *meshes[ i ]->material;
		
		cVector4f color{ 1.0f, 1.0f, 1.0f, 1.0f };

		material.bind();

		material.shader->ubBegin();
		material.shader->ubBufferData( "uProj",  glm::value_ptr( projection ), 64 );
		material.shader->ubBufferData( "uView",  glm::value_ptr( view ),       64 );
		material.shader->ubBufferData( "uModel", glm::value_ptr( model ),      64 );
		material.shader->ubBufferData( "uColor", &color, sizeof( cVector4f ) );
		material.shader->ubEnd();

		backend->bindVertexArray( meshes[ i ]->vertex_array );
		backend->drawElements( meshes[ i ]->num_indices, cm::eDrawMode::DrawMode_Triangle );

		//material.unbind();
	}

	backend->bindVertexArray( 0 );
}
