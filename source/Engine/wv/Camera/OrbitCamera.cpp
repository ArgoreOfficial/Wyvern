#include "OrbitCamera.h"

#include <wv/Device/Context.h>
#include <wv/Application/Application.h>
#include <wv/Math/Math.h>

wv::OrbitCamera::OrbitCamera( CameraType _type, float _fov, float _near, float _far ) :
	ICamera( _type, _fov, _near, _far )
{

}

wv::OrbitCamera::~OrbitCamera( void )
{

}

void wv::OrbitCamera::onCreate()
{
	subscribeMouseEvents();
	subscribeInputEvent();
}

void wv::OrbitCamera::onMouseEvent( MouseEvent _event )
{
	wv::Context* ctx = wv::Application::getApplication()->context;
	
	if ( (_event.buttondown || _event.buttonup) && _event.button == MouseEvent::WV_MOUSE_BUTTON_LEFT )
	{
		m_old_mouse_pos = _event.position;
		m_input_enabled = _event.buttondown;

		// reset input
		m_rotate = { 0.0f, 0.0f };
		
		return;
	}

	if ( !m_input_enabled )
		return;

	int delta_x = _event.position.x - m_old_mouse_pos.x;
	int delta_y = _event.position.y - m_old_mouse_pos.y;
	m_old_mouse_pos = _event.position;

	m_rotate = wv::Vector2f( (float)delta_x, (float)delta_y );
}

void wv::OrbitCamera::onInputEvent( InputEvent _event )
{
	return;

	/// scroll?

	int button_delta = _event.buttondown ? 1 : -1;

	if ( !m_input_enabled )
		return;

	if ( !_event.repeat )
	{
		switch ( _event.key )
		{
		//case GLFW_KEY_W: m_move.z += -button_delta; break;
		//case GLFW_KEY_S: m_move.z +=  button_delta; break;
		//case GLFW_KEY_A: m_move.x += -button_delta; break;
		//case GLFW_KEY_D: m_move.x +=  button_delta; break;
		//case GLFW_KEY_E: m_move.y +=  button_delta; break; // up
		//case GLFW_KEY_Q: m_move.y += -button_delta; break; // down
		//
		//case GLFW_KEY_LEFT_SHIFT:   m_speed += button_delta * 200.0f; break;
		}
	}
}

void wv::OrbitCamera::update( double _delta_time )
{
	m_transform.rotation.y += m_rotate.x * 0.4f;
	m_transform.rotation.x -= m_rotate.y * 0.4f;

	if ( m_transform.rotation.x > 89.0f )
		m_transform.rotation.x = 89.0f;
	if ( m_transform.rotation.x < -89.0f )
		m_transform.rotation.x = -89.0f;

	float d = cos( wv::Math::degToRad( -m_transform.rotation.x ) ) * 4.0f;
	m_transform.position.x = sin( wv::Math::degToRad( -m_transform.rotation.y ) ) * d;
	m_transform.position.y = sin( wv::Math::degToRad( -m_transform.rotation.x ) ) * 4.0f;
	m_transform.position.z = cos( wv::Math::degToRad( -m_transform.rotation.y ) ) * d;
	
	m_rotate = { 0.0f, 0.0f };
}