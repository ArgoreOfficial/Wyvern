#include "EnemyShip.h"

#include <wv/Application/Application.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Camera/ICamera.h>
#include <wv/Math/Math.h>

EnemyShip::EnemyShip( wv::Mesh* _mesh ) :
	Ship{ _mesh }
{
	m_maxSpeed = 1.0f;
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
	
	/* move */

	//wv::Vector3f rel = m_targetRotation - m_transform.rotation;
	//
	//if ( rel.y >  180.0f ) 
	//	rel.y -= 360.0f;
	//if ( rel.y < -180.0f ) 
	//	rel.y += 360.0f;

	//float roll = rel.y * (float)_deltaTime * 400.0f;
	//roll = wv::Math::clamp( roll, -80.0f, 80.0f );

	//m_transform.rotate( rel * (float)_deltaTime * 4.0f );
	//if ( m_transform.rotation.y >  180.0f ) m_transform.rotation.y -= 360.0f;
	//if ( m_transform.rotation.y < -180.0f ) m_transform.rotation.y += 360.0f;
	//
	//m_transform.rotation.z = roll;

	Ship::update( _deltaTime );
}
