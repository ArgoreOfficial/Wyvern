#include "OrbitCamera.h"

#include <wv/Device/DeviceContext.h>
#include <wv/Application/Application.h>
#include <wv/Math/Math.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::OrbitCamera::OrbitCamera( CameraType _type, float _fov, float _near, float _far ) :
	ICamera( _type, _fov, _near, _far )
{
	m_transform.setRotation( { -15.0f, 225.0f, 0.0f } );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::OrbitCamera::~OrbitCamera( void )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OrbitCamera::onCreate()
{
	subscribeMouseEvents();
	subscribeInputEvent();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OrbitCamera::onMouseEvent( MouseEvent _event )
{
	wv::DeviceContext* ctx = wv::Application::get()->context;
	
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

///////////////////////////////////////////////////////////////////////////////////////

void wv::OrbitCamera::onInputEvent( InputEvent _event )
{

	/// scroll?

}

///////////////////////////////////////////////////////////////////////////////////////

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