#include "Model.h"
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Rendering/Camera.h>

using namespace WV;

Model::Model( Mesh* _mesh, Material* _material ) :
	m_mesh(_mesh), m_material(_material)
{
}

Model::~Model()
{
	delete m_material;
}

void WV::Model::bind( Camera& _camera )
{

	glm::mat4 model;
	model = glm::translate( model, m_position );
	model = glm::rotate( model, glm::radians( m_rotation.x ), glm::vec3( 1.0, 0.0, 0.0 ) );
	model = glm::rotate( model, glm::radians( m_rotation.y ), glm::vec3( 0.0, 1.0, 0.0 ) );
	model = glm::rotate( model, glm::radians( m_rotation.z ), glm::vec3( 0.0, 0.0, 1.0 ) );

	m_material->bind( _camera.getProjectionMatrix(), _camera.getViewMatrix(), model );

	m_mesh->getVertexArray()->bind();
	m_mesh->getIndexBuffer()->bind();
}
