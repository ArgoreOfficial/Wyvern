#include "cModel.h"

#include <cm/Core/stb_image.h>
#include <wv/Core/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <wv/Graphics/Primitives.h>
#include <wv/Manager/cContentManager.h>
#include <wv/Core/cApplication.h>

#include <cm/Core/cWindow.h>
#include <wv/Camera/iCamera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void wv::cModel::create( std::string _path )
{
	
}

void wv::cModel::render()
{
	cm::iBackend* backend = cRenderer::getInstance().getBackend();
	cm::cWindow* window = cApplication::getInstance().getWindow();

	iCamera* camera = cApplication::getInstance().m_current_camera;

	float w = (float)window->getWidth();
	float h = (float)window->getHeight();

	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 view = camera->getViewMatrix();

	glm::mat4 model = transform.getMatrix();

	for ( int i = 0; i < meshes.size(); i++ )
	{
		cMaterial& material = *meshes[ i ]->material;
		
		material.bind();

		material.shader->uniformBlockBegin();
		material.shader->uniformBlockBuffer( "uProj",  glm::value_ptr( projection ), 64 );
		material.shader->uniformBlockBuffer( "uView",  glm::value_ptr( view ),       64 );
		material.shader->uniformBlockBuffer( "uModel", glm::value_ptr( model ),      64 );
		material.shader->uniformBlockEnd();

		backend->bindVertexArray( meshes[ i ]->vertex_array );
		backend->drawElements( meshes[ i ]->num_vertices, cm::eDrawMode::DrawMode_Triangle );

		material.unbind();
	}

	backend->bindVertexArray( 0 );
}
