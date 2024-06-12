#include "EnemyShip.h"

#include <wv/Application/Application.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Camera/ICamera.h>
#include <wv/Math/Math.h>

EnemyShip::EnemyShip( wv::Mesh* _mesh ) :
	Ship{ _mesh }
{
	m_maxSpeed = 100.0f;
}

EnemyShip::~EnemyShip()
{

}

void EnemyShip::onCreate()
{
	wv::Application* app = wv::Application::get();
	
}

void EnemyShip::update( double _deltaTime )
{
	wv::Vector3f dir = m_transform.position - m_target;
	dir.normalize();
	m_targetRotation = dir.directionToEuler();
	// m_targetRotation.x = wv::Math::clamp( m_targetRotation.x, -70.0f, 70.0f );

	wv::Vector3f rel = m_targetRotation - m_transform.rotation;
	
	if ( rel.y >  180.0f ) 
		rel.y -= 360.0f;
	if ( rel.y < -180.0f ) 
		rel.y += 360.0f;

	//rel.y = wv::Math::clamp( rel.y, -15.0f, 15.0f );
	
	float roll = rel.y * 0.3f;
	roll = wv::Math::clamp( roll, -80.0f, 80.0f );

	m_transform.rotate( rel * (float)_deltaTime * 4.0f );
	if ( m_transform.rotation.y >  180.0f ) m_transform.rotation.y -= 360.0f;
	if ( m_transform.rotation.y < -180.0f ) m_transform.rotation.y += 360.0f;
	
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

	Ship::update( _deltaTime );
}
