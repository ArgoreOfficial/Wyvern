#include "iCamera.h"

#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>

wv::iCamera::iCamera( eCameraType _type, float _fov, float _near, float _far ) :
	m_type{ _type },
	m_fov{ _fov },
	m_near{ _near },
	m_far{ _far }
{

}

glm::mat4 wv::iCamera::getProjectionMatrix( void )
{
	switch ( m_type )
	{
	case CameraType_Perspective:  return getPerspectiveMatrix();  break;
	case CameraType_Orthographic: return getOrthographicMatrix(); break;
	}

	return glm::mat4( 1.0f );
}

glm::mat4 wv::iCamera::getPerspectiveMatrix( void )
{
	wv::cRenderer& renderer = cRenderer::getInstance();
	return glm::perspective( glm::radians( m_fov ), renderer.getViewportAspect(), m_near, m_far );
}

glm::mat4 wv::iCamera::getOrthographicMatrix( void )
{
	wv::cRenderer& renderer = cRenderer::getInstance();
	float w = (float)renderer.getViewportWidth () / 2.0f;
	float h = (float)renderer.getViewportHeight() / 2.0f;

	return glm::ortho( -w, w, -h, h, -1000.0f, 1000.0f );
}

glm::mat4 wv::iCamera::getViewMatrix( void )
{
	glm::vec3 pos{ m_transform.position.x, m_transform.position.y, m_transform.position.z };

	float yaw = glm::radians( m_transform.rotation.y - 90.0f );
	float pitch = glm::radians( m_transform.rotation.x );

	glm::vec3 direction;
	direction.x = cos( yaw ) * cos( pitch );
	direction.y = sin( pitch );
	direction.z = sin( yaw ) * cos( pitch );

	return glm::lookAt( pos, pos + glm::normalize( direction ), glm::vec3( 0.0, 1.0, 0.0 ) );
}

wv::cVector3f wv::iCamera::getViewDirection()
{
	float yaw = glm::radians( m_transform.rotation.y - 90.0f );
	float pitch = glm::radians( m_transform.rotation.x );

	cVector3f direction;
	direction.x = cos( yaw ) * cos( pitch );
	direction.y = sin( pitch );
	direction.z = sin( yaw ) * cos( pitch );
    return direction;
}
