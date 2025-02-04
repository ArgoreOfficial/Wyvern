#include "OrbitCamera.h"

#include <wv/Device/DeviceContext.h>
#include <wv/Engine/Engine.h>
#include <wv/Math/Math.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::OrbitCamera::OrbitCamera( CameraType _type, float _fov, float _near, float _far ) :
	iCamera( _type, _fov, _near, _far )
{
	m_transform.setRotation( { -15.0f, 0.0f, 0.0f } );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::OrbitCamera::~OrbitCamera( void )
{

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OrbitCamera::onEnter()
{
	m_inputListener.hook();
	m_mouseListener.hook();
}

void wv::OrbitCamera::handleInput()
{
	sMouseEvent mouseEvent;
	while ( m_mouseListener.pollEvent( mouseEvent ) )
	{
		if ( ( mouseEvent.buttondown || mouseEvent.buttonup ) && mouseEvent.button == sMouseEvent::WV_MOUSE_BUTTON_LEFT )
		{
			m_input_enabled = mouseEvent.buttondown;

			// reset input
			m_rotate = { 0.0f, 0.0f };

			return;
		}

		if ( !m_input_enabled )
			return;

		m_rotate = wv::Vector2f( (float)mouseEvent.delta.x, (float)mouseEvent.delta.y );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::OrbitCamera::update( double _delta_time )
{
	handleInput();

	m_transform.rotation.y -= m_rotate.x * 0.4f;
	m_transform.rotation.x -= m_rotate.y * 0.4f;

	if ( m_transform.rotation.x > 89.0f )
		m_transform.rotation.x = 89.0f;
	if ( m_transform.rotation.x < -89.0f )
		m_transform.rotation.x = -89.0f;

	float d = std::cos( wv::Math::radians( -m_transform.rotation.x ) ) * 4.0f;
	m_transform.position.x = std::sin( wv::Math::radians( m_transform.rotation.y ) ) * d;
	m_transform.position.y = std::sin( wv::Math::radians( -m_transform.rotation.x ) ) * 4.0f;
	m_transform.position.z = std::cos( wv::Math::radians( m_transform.rotation.y ) ) * d;
	
	m_rotate = { 0.0f, 0.0f };

	iCamera::update( _delta_time );
}