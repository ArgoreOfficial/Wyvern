#include "Camera.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/DeviceContext.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::ICamera::ICamera( CameraType _type, float _fov, float _near, float _far ) :
	m_type{ _type },
	fov{ _fov },
	m_near{ _near },
	m_far{ _far }
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

glm::mat4x4 wv::ICamera::getProjectionMatrix( void )
{
	switch ( m_type )
	{
	case WV_CAMERA_TYPE_PERSPECTIVE:  return getPerspectiveMatrix();  break;
	case WV_CAMERA_TYPE_ORTHOGRAPHIC: return getOrthographicMatrix(); break;
	}

	return glm::mat4x4( 1.0f );
}

///////////////////////////////////////////////////////////////////////////////////////

glm::mat4x4 wv::ICamera::getPerspectiveMatrix( void )
{
	wv::iDeviceContext* ctx = wv::cEngine::get()->context;
	return glm::perspective( glm::radians( fov ), ctx->getAspect(), m_near, m_far);
}

///////////////////////////////////////////////////////////////////////////////////////

glm::mat4x4 wv::ICamera::getOrthographicMatrix( void )
{
	wv::iDeviceContext* ctx = wv::cEngine::get()->context;
	float w = (float)ctx->getWidth()  / 2.0f;
	float h = (float)ctx->getHeight() / 2.0f;

	return glm::ortho( -w, w, -h, h, -1000.0f, 1000.0f );
}

///////////////////////////////////////////////////////////////////////////////////////

glm::mat4 wv::ICamera::getViewMatrix( void )
{
	glm::vec3 pos{ m_transform.position.x, m_transform.position.y, m_transform.position.z };

	float yaw = glm::radians( m_transform.rotation.y - 90.0f );
	float pitch = glm::radians( m_transform.rotation.x );

	glm::vec3 direction;
	direction.x = cos( yaw ) * cos( pitch );
	direction.y = sin( pitch );
	direction.z = sin( yaw ) * cos( pitch );
	
	return glm::inverse( m_transform.getMatrix() ); 
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::ICamera::getViewDirection()
{
	float yaw = glm::radians( m_transform.rotation.y - 90.0f );
	float pitch = glm::radians( m_transform.rotation.x );

	Vector3f direction;
	direction.x = cos( yaw ) * cos( pitch );
	direction.y = sin( pitch );
	direction.z = sin( yaw ) * cos( pitch );
    return direction;
}
