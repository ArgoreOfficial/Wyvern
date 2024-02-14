#include "cCamera.h"

#include <cm/Core/cWindow.h>
#include <wv/Core/cApplication.h>

wv::cCamera::cCamera( eCameraType _type, float _fov, float _near, float _far ) :
	m_type{ _type },
	m_fov{ _fov },
	m_near{ _near },
	m_far{ _far }
{

}

wv::cCamera::~cCamera( void )
{
}

void wv::cCamera::onRawInput( sInputInfo* _info )
{
	int button_delta = _info->buttondown ? 1 : -1;

	if ( _info->type == sInputInfo::InputInfo_Key && !_info->repeat )
	{
		switch ( _info->key )
		{
		case GLFW_KEY_W: m_move.z += -button_delta; break;
		case GLFW_KEY_S: m_move.z +=  button_delta; break;
		case GLFW_KEY_A: m_move.x += -button_delta; break;
		case GLFW_KEY_D: m_move.x +=  button_delta; break;
		}
	}
	else if( _info->type == sInputInfo::InputInfo_Mouse )
	{
		
		cm::cWindow* window = cApplication::getInstance().getWindow();
		int delta_x = _info->mouse_position.x - m_old_mouse_pos.x;
		int delta_y = _info->mouse_position.y - m_old_mouse_pos.y;
		m_old_mouse_pos = _info->mouse_position;

		if( abs(delta_x) < 500 && abs( delta_y ) < 500 )
			m_rotate = wv::cVector2f( (float)delta_x, (float)delta_y );
	}
}

void wv::cCamera::update( double _delta_time )
{
	m_transform.rotation.y += m_rotate.x * 100.0f * (float)_delta_time;
	m_transform.rotation.x -= m_rotate.y * 100.0f * (float)_delta_time;
	
	float yaw   = glm::radians( m_transform.rotation.y );
	float pitch = glm::radians( m_transform.rotation.x );

	float move_z =  cos( yaw ) * m_move.z + sin( yaw ) * m_move.x;
	float move_x = -sin( yaw ) * m_move.z + cos( yaw ) * m_move.x;

	m_transform.translate( wv::cVector3f{ move_x, 0.0f, move_z } * 2.0f * (float)_delta_time );

	m_rotate = { 0.0f, 0.0f };
}

glm::mat4 wv::cCamera::getProjectionMatrix( void )
{
	switch ( m_type )
	{
	case CameraType_Perspective:  return getPerspectiveMatrix();  break;
	case CameraType_Orthographic: return getOrthographicMatrix(); break;
	}

	return glm::mat4( 1.0f );
}

glm::mat4 wv::cCamera::getPerspectiveMatrix( void )
{
	cm::cWindow* window = cApplication::getInstance().getWindow();
	return glm::perspective( glm::radians( m_fov ), window->getAspect(), m_near, m_far );
}

glm::mat4 wv::cCamera::getOrthographicMatrix( void )
{
	cm::cWindow* window = cApplication::getInstance().getWindow();
	float w = (float)window->getWidth () / 2.0f;
	float h = (float)window->getHeight() / 2.0f;

	return glm::ortho( -w, w, -h, h, -1000.0f, 1000.0f );
}

glm::mat4 wv::cCamera::getViewMatrix( void )
{
	glm::vec3 pos{ m_transform.position.x, m_transform.position.y, m_transform.position.z };

	float yaw = glm::radians( m_transform.rotation.y - 90.0f ); // TODO: change from radians to degrees?
	float pitch = glm::radians( m_transform.rotation.x );

	glm::vec3 direction;
	direction.x = cos( yaw ) * cos( pitch );
	direction.y = sin( pitch );
	direction.z = sin( yaw ) * cos( pitch );

	return glm::lookAt( pos, pos + glm::normalize( direction ), glm::vec3( 0.0, 1.0, 0.0 ) );
}
