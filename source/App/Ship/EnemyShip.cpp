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
	wv::Vector3f dir;
	if( m_chasing )
		dir = m_transform.position - m_target;
	else
	{
		dir = m_target - m_transform.position;
		m_chase_timer += _deltaTime;

		if ( m_chase_timer > 5.0f )
		{
			m_switching = true;
			m_chasing = true;

			m_chase_timer = 0.0f;
			printf( "Switching\n" );
		}
	}
	
	dir.normalize();
	m_targetRotation = dir.directionToEuler();

	m_targetRotation.x = wv::Math::clamp( m_targetRotation.x, -70.0f, 70.0f );
	
	wv::Vector3f rel = m_targetRotation - m_transform.rotation;
	
	// if ( rel.y >  180.0f ) rel.y -= 180.0f;
	// if ( rel.y < -180.0f ) rel.y += 180.0f;
	
	if ( !m_switching )
	{
		if ( rel.y > 140.0f || rel.y < -140.0f )
		{
			m_chasing = !m_chasing;
			printf( "Chased\n" );
		}
	}
	else
	{
		if ( rel.y > 90.0f || rel.y < -90.0f )
			rel.y *= -1.0f;
		
		printf( "%f\n", rel.y );

		if ( rel.y < 20.0f && rel.y > -20.0f )
		{
			m_switching = false;
			printf( "Done switching\n" );
		}
	}
	
	rel.y = wv::Math::clamp( rel.y, -15.0f, 15.0f );
	
	float roll = rel.y * 0.3f;
	roll = wv::Math::clamp( roll, -80.0f, 80.0f );

	m_transform.rotate( rel * (float)_deltaTime * 4.0f );
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
