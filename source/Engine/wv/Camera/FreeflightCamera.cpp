#include "FreeflightCamera.h"

#include <wv/Device/DeviceContext.h>
#include <wv/Application/Application.h>

//#include <cm/Core/cWindow.h>
//#include <wv/Core/cApplication.h>

#include <SDL2/SDL_keycode.h> /// TODO: remove

wv::FreeflightCamera::FreeflightCamera( CameraType _type, float _fov, float _near, float _far ) :
	ICamera( _type, _fov, _near, _far )
{

}

wv::FreeflightCamera::~FreeflightCamera( void )
{

}

void wv::FreeflightCamera::onCreate()
{
	subscribeMouseEvents();
	subscribeInputEvent();
}

void wv::FreeflightCamera::onMouseEvent( MouseEvent _event )
{
	wv::DeviceContext* ctx = wv::Application::get()->context;
	
	if ( _event.buttondown && _event.button == MouseEvent::WV_MOUSE_BUTTON_RIGHT )
	{
		m_freecam_enabled = !m_freecam_enabled;
		ctx->setMouseLock( m_freecam_enabled );

		// reset input
		m_rotate = { 0.0f, 0.0f };
		m_move = { 0.0f, 0.0f, 0.0f };
		m_speed = m_speed_normal;
	}

	if ( !m_freecam_enabled )
		return;

	m_rotate = { (float)_event.delta.x, (float)_event.delta.y };
}

void wv::FreeflightCamera::onInputEvent( InputEvent _event )
{
	int button_delta = _event.buttondown ? 1 : -1;

	if ( !m_freecam_enabled )
		return;

	if ( !_event.repeat )
	{
			/// TODO: change to WV_KEY
		switch ( _event.key )
		{ 
		case 'W': m_move.z += -button_delta; break;
		case 'S': m_move.z += button_delta; break;
		case 'A': m_move.x += -button_delta; break;
		case 'D': m_move.x += button_delta; break;
		case 'E': m_move.y += button_delta; break; // up
		case 'Q': m_move.y += -button_delta; break; // down

		case SDL_Scancode::SDL_SCANCODE_LSHIFT: m_speed += button_delta * 200.0f; break;
		}
	}
}

void wv::FreeflightCamera::update( double _delta_time )
{
	m_velocity *= 1.0f - (float)_delta_time * 10.0f;

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

	Vector3f move = Vector3f{ forward.x, forward.y, forward.z } * -m_move.z;
	move += Vector3f{ right.x, right.y, right.z } * m_move.x;
	move += Vector3f{ up.x, up.y, up.z } * m_move.y;

	Vector3f acceleration = move * m_speed;

	m_transform.position += m_velocity * (float)_delta_time + acceleration * 0.5f * (float)_delta_time * (float)_delta_time;
	m_velocity += acceleration * (float)_delta_time;

	m_rotate = { 0.0f, 0.0f };
}