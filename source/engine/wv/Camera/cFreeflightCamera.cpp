#include "cFreeflightCamera.h"

#include <cm/Core/cWindow.h>
#include <wv/Core/cApplication.h>

wv::cFreeflightCamera::cFreeflightCamera( eCameraType _type, float _fov, float _near, float _far ) :
	iCamera( _type, _fov, _near, _far )
{

}

wv::cFreeflightCamera::~cFreeflightCamera( void )
{

}

void wv::cFreeflightCamera::onCreate()
{
	subscribeMouseEvents();
	subscribeInputEvent();
}

void wv::cFreeflightCamera::onMouseEvent( sMouseEvent _event )
{
	cm::cWindow* window = cApplication::getInstance().getWindow();
	
	if ( (_event.button_down || _event.button_up) && _event.button == sMouseEvent::MouseButton_Right )
	{
		window->setMouseLock( _event.button_down );
		m_old_mouse_pos = _event.position;
		m_freecam_enabled = _event.button_down;

		// reset input
		m_rotate = { 0.0f, 0.0f };
		m_move = { 0.0f, 0.0f, 0.0f };
		return;
	}

	if ( !m_freecam_enabled )
		return;

	int delta_x = _event.position.x - m_old_mouse_pos.x;
	int delta_y = _event.position.y - m_old_mouse_pos.y;
	m_old_mouse_pos = _event.position;

	if ( abs( delta_x ) < 500 && abs( delta_y ) < 500 )
		m_rotate = wv::cVector2f( (float)delta_x, (float)delta_y );

}

void wv::cFreeflightCamera::onInputEvent( sInputEvent _event )
{
	int button_delta = _event.buttondown ? 1 : -1;

	if ( !m_freecam_enabled )
		return;

	if ( !_event.repeat )
	{
		switch ( _event.key )
		{
		case GLFW_KEY_W:            m_move.z += -button_delta; break;
		case GLFW_KEY_S:            m_move.z += button_delta; break;
		case GLFW_KEY_A:            m_move.x += -button_delta; break;
		case GLFW_KEY_D:            m_move.x += button_delta; break;
		case GLFW_KEY_SPACE:        m_move.y += button_delta; break;
		case GLFW_KEY_LEFT_CONTROL: m_move.y += -button_delta; break;

		case GLFW_KEY_LEFT_SHIFT:   m_speed += button_delta * 7.0f; break;
		}
	}
}

void wv::cFreeflightCamera::update( double _delta_time )
{
	m_transform.rotation.y += m_rotate.x * 0.1f;
	m_transform.rotation.x -= m_rotate.y * 0.1f;
	
	if ( m_transform.rotation.x > 89.0f )
		m_transform.rotation.x = 89.0f;
	if ( m_transform.rotation.x < -89.0f )
		m_transform.rotation.x = -89.0f;

	float yaw   = glm::radians( m_transform.rotation.y );
	float pitch = glm::radians( m_transform.rotation.x );
	float roll  = 0.0f; // glm::radians( 0.0f );


	// TODO: change to wv::matrix

	// forward
	glm::mat4 rot_forward( 1.0f );
	rot_forward = glm::rotate( rot_forward, -yaw, { 0.0f, 1.0f, 0.0f } );
	rot_forward = glm::rotate( rot_forward, pitch, { 1.0f, 0.0f, 0.0f } );
	glm::vec4 forward = rot_forward * glm::vec4{ 0.0f, 0.0f, -1.0f, 1.0f };;
	
	// right
	glm::mat4 rot_right( 1.0f );
	rot_right = glm::rotate( rot_right, -yaw,  { 0.0f, 1.0f, 0.0f } );
	//rot_right = glm::rotate( rot_right, pitch, { 1.0f, 0.0f, 0.0f } );
	glm::vec4 right = rot_right * glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f };;

	// up
	glm::vec4 up = rot_forward * glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f };;

	cVector3f move = cVector3f{ forward.x, forward.y, forward.z } * -m_move.z;
	move += cVector3f{ right.x, right.y, right.z } * m_move.x;
	move += cVector3f{ up.x, up.y, up.z } * m_move.y;

	m_transform.translate( move * m_speed * (float)_delta_time );

	m_rotate = { 0.0f, 0.0f };
}