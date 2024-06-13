#include "PlayerShip.h"

#include <wv/Application/Application.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Camera/ICamera.h>
#include <wv/Math/Math.h>
#include <wv/Device/AudioDevice.h>

PlayerShip::PlayerShip( wv::Mesh* _mesh ) :
	Ship{ _mesh }
{
	m_camera = new wv::ICamera( wv::ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	m_maxSpeed = 4.0f;
	m_throttle = 1.0f;
}

PlayerShip::~PlayerShip()
{

}

void PlayerShip::onCreate()
{
	wv::Application* app = wv::Application::get();
	
	subscribeMouseEvents();
	subscribeInputEvent();

	app->currentCamera = m_camera;
	app->context->setMouseLock( true );

	m_camera->getTransform().position = { 0.0f, 3.0f, 28.0f };
	m_camera->getTransform().parent = &m_transform;

	m_engineSound = app->audio->loadAudio2D( "x-wing-engine.flac" );
	m_engineSound->play( 0.4f, true );
}

void PlayerShip::onMouseEvent( wv::MouseEvent _event )
{
	m_aimInput.x += (float)_event.delta.x;
	m_aimInput.y += (float)_event.delta.y;
	m_usingKeyboard = false;
}

void PlayerShip::onInputEvent( wv::InputEvent _event )
{
	if ( !_event.repeat )
	{
		float p = (int)_event.buttondown - (int)_event.buttonup;
		switch ( _event.key )
		{
		case 'Z': m_throttleInput += p; break;
		case 'X': m_throttleInput -= p; break;

		case 'W': m_aimInput.y -= p * ( m_invertPitch ? -1.0f : 1.0f ); m_usingKeyboard = true; break;
		case 'S': m_aimInput.y += p * ( m_invertPitch ? -1.0f : 1.0f ); m_usingKeyboard = true; break;
		case 'A': m_aimInput.x -= p; m_usingKeyboard = true; break;
		case 'D': m_aimInput.x += p; m_usingKeyboard = true; break;

		case 'I': if ( _event.buttondown ) { m_invertPitch = !m_invertPitch; } break;
		}
	}
}

void PlayerShip::update( double _deltaTime )
{
	wv::Application* app = wv::Application::get();
	app->audio->setListenerPosition( m_transform.position );
	app->audio->setListenerDirection( -m_transform.rotation.eulerToDirection() );

	m_throttle += (float)m_throttleInput * _deltaTime;
	m_throttle = wv::Math::clamp( m_throttle, 0.0f, 1.0f );
	wv::Vector2f input = m_aimInput;

	if ( m_usingKeyboard )
		input *= _deltaTime * 900.0f;
	else
		input *= 1.3f;

	m_targetRotation -= wv::Vector3f{ input.y, input.x, 0.0f } * 0.1f;
	m_targetRotation.x = wv::Math::clamp( m_targetRotation.x, -80.0f, 80.0f );

	if( !m_usingKeyboard )
		m_aimInput = {};
	

	wv::Vector3f rel = m_targetRotation - m_transform.rotation;
	rel.x *= -0.3f;
	rel.y *=  0.3f;

	Ship::update( _deltaTime );
	
	m_camera->getTransform().setPosition( ( rel + wv::Vector3f{-10.0f, 0.0f, 0.0f} ).eulerToDirection() * 0.4f );
	m_camera->getTransform().setRotation( { rel.x, rel.y, 0.0f } );
}
