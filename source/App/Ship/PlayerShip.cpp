#include "PlayerShip.h"

#include <wv/Application/Application.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Camera/ICamera.h>
#include <wv/Math/Math.h>

PlayerShip::PlayerShip( wv::Mesh* _mesh ) :
	Ship{ _mesh }
{
	m_camera = new wv::ICamera( wv::ICamera::WV_CAMERA_TYPE_PERSPECTIVE );
	m_maxSpeed = 100.0f;
	m_throttle = 0.0f;
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
}

void PlayerShip::onMouseEvent( wv::MouseEvent _event )
{
	m_aimInput.x += (float)_event.delta.x;
	m_aimInput.y += (float)_event.delta.y;
}

void PlayerShip::onInputEvent( wv::InputEvent _event )
{
	if ( !_event.repeat )
	{
		float p = (int)_event.buttondown - (int)_event.buttonup;
		switch ( _event.key )
		{
		case 'W': m_throttleInput += p; break;
		case 'S': m_throttleInput -= p; break;
		}
	}
}

void PlayerShip::update( double _deltaTime )
{
	m_throttle += (float)m_throttleInput * _deltaTime;
	m_throttle = wv::Math::clamp( m_throttle, 0.0f, 1.0f );
	//printf( "In %i   Throttle: %f\n", m_throttleInput, m_throttle );

	

	m_targetRotation -= wv::Vector3f{ m_aimInput.y, m_aimInput.x, 0.0f } * 0.1f;
	m_targetRotation.x = wv::Math::clamp( m_targetRotation.x, -70.0f, 70.0f );
	m_aimInput = {};
	
	float roll = ( m_targetRotation.y - m_transform.rotation.y ) * 1.2f;
	roll = wv::Math::clamp( roll, -80.0f, 80.0f );

	m_transform.rotate( ( m_targetRotation - m_transform.rotation ) * (float)_deltaTime * 4.0f );
	m_transform.rotation.z = roll;
	m_cameraRotation += ( m_transform.rotation - m_cameraRotation ) * (float)_deltaTime * 12.0f;


	wv::Vector3f camOffset = m_cameraRotation;
	camOffset.x -= 8.0f;
	wv::Vector3f camPos = camOffset.eulerToDirection();
	camPos.x *= 30.0f;
	camPos.y *= 30.0f;
	camPos.z *= 30.0f;

	wv::Vector3f camRot = m_cameraRotation;
	camRot.y *= -1.0f;

	m_camera->getTransform().position = m_transform.position + camPos;
	m_camera->getTransform().rotation = camRot;

	Ship::update( _deltaTime );
}
