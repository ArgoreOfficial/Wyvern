#include "freeflight_camera.h"

#include <wv/device/device_context.h>
#include <wv/engine.h>

#include <wv/math/matrix.h>

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////

wv::FreeflightCamera::FreeflightCamera( CameraType _type, float _fov, float _near, float _far ) :
	ICamera( _type, _fov, _near, _far )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::FreeflightCamera::onEnter()
{
	m_inputListener.hook();
	m_mouseListener.hook();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::FreeflightCamera::handleInput()
{
	MouseEvent mouseEvent;
	while ( m_mouseListener.pollEvent( mouseEvent ) )
	{
		wv::IDeviceContext* ctx = wv::Engine::get()->context;

		if ( mouseEvent.buttondown && mouseEvent.button == MouseEvent::WV_MOUSE_BUTTON_RIGHT )
		{
			m_freecam_enabled = !m_freecam_enabled;
			ctx->setMouseLock( m_freecam_enabled );
		}

		m_rotate += { -(float)mouseEvent.delta.x, (float)mouseEvent.delta.y };
	}


	InputEvent inputEvent;
	while ( m_inputListener.pollEvent( inputEvent ) )
	{
		int button_delta = inputEvent.buttondown ? 1 : -1;

		if ( !inputEvent.repeat )
		{
			switch ( inputEvent.key )
			{
			case 'W' /* WV_KEY_W */: m_move.z += -button_delta; break;
			case 'S' /* WV_KEY_S */: m_move.z +=  button_delta; break;
			case 'A' /* WV_KEY_A */: m_move.x += -button_delta; break;
			case 'D' /* WV_KEY_D */: m_move.x +=  button_delta; break;
			case 'E' /* WV_KEY_E */: m_move.y +=  button_delta; break; // up
			case 'Q' /* WV_KEY_Q */: m_move.y += -button_delta; break; // down

			case WV_KEY_LEFT_SHIFT:
				m_speed += button_delta * 1000.0f;
				break;

			default: 
				break;

			}
		}
	}
}

void wv::FreeflightCamera::update( double _delta_time )
{
	handleInput();

	m_velocity *= 1.0f - (float)_delta_time * 10.0f;

	if (m_freecam_enabled)
	{
		m_transform.rotation.y += m_rotate.x * 0.08f;
		m_transform.rotation.x -= m_rotate.y * 0.08f;
	}

	if ( m_transform.rotation.x > 89.0f )
		m_transform.rotation.x = 89.0f;
	if ( m_transform.rotation.x < -89.0f )
		m_transform.rotation.x = -89.0f;

	float yaw   = Math::radians( m_transform.rotation.y );
	float pitch = Math::radians( m_transform.rotation.x );
	
	// forward
	Matrix4x4f rot_forward( 1.0f );
	rot_forward = Math::rotateY( rot_forward, yaw );
	rot_forward = Math::rotateX( rot_forward, pitch );

	Vector4f forward = Vector4f{ 0.0f, 0.0f, -1.0f, 1.0f } * rot_forward;
	
	// right
	Matrix4x4f rot_right( 1.0f );
	rot_right = Math::rotateY( rot_right, yaw );
	Vector4f right = Vector4f{ 1.0f, 0.0f, 0.0f, 1.0f } * rot_right;
	
	// up
	Vector4f up = Vector4f{ 0.0f, 1.0f, 0.0f, 1.0f } * rot_forward;

	Vector3f move = {};
	if ( m_freecam_enabled )
	{
		move += Vector3f{ forward.x, forward.y, forward.z } * -m_move.z;
		move += Vector3f{ right.x, right.y, right.z } * m_move.x;
		move += Vector3f{ up.x, up.y, up.z } * m_move.y;
	}

	Vector3f acceleration = move * m_speed;

	m_transform.position += m_velocity * (float)_delta_time + acceleration * 0.5f * (float)_delta_time * (float)_delta_time;
	m_velocity += acceleration * (float)_delta_time;

	m_rotate = { 0.0f, 0.0f };

	ICamera::update( _delta_time );
}