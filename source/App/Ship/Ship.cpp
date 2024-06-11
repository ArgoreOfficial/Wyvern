#include "Ship.h"
#include <wv/Application/Application.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Assets/Materials/Material.h>

Ship::Ship( wv::Mesh* _mesh )
{
	m_mesh = _mesh;
}

Ship::~Ship()
{

}

void Ship::update( double _deltaTime )
{
	wv::Vector3f forward = m_transform.forward();
	m_transform.position -= forward * m_throttle * m_maxSpeed * _deltaTime;
}

void Ship::draw( wv::GraphicsDevice* _device )
{
	if ( !m_mesh )
		return;

	m_mesh->transform = m_transform;

	_device->draw( m_mesh );
}
