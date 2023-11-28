#include "cCamera.h"
#include <Wyvern/cApplication.h>
using namespace wv;

cCamera::cCamera()
{

	m_aspect = cApplication::getViewport().getAspect();

}

wv::cCamera::~cCamera()
{

}

void wv::cCamera::update()
{

	glm_proj = glm::perspective( glm::radians( m_fov ), m_aspect, 0.1f, 100.0f );
	glm::vec3 pos{ m_position.x, m_position.y, m_position.z };

	glm::mat4 view = glm::mat4( 1.0f );
	
	glm::vec3 direction;
	float yaw = m_rotation.y;
	float pitch = m_rotation.x;

	direction.x = cos( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
	direction.y = sin( glm::radians( pitch ) );
	direction.z = sin( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );

	glm::vec3 cameraPos = glm::vec3( m_position.x, m_position.y, m_position.z );
	glm::vec3 cameraUp = glm::vec3( 0.0f, 1.0f, 0.0f );
	view = glm::lookAt( cameraPos, cameraPos + direction, cameraUp );

	glm_view = view;
}

