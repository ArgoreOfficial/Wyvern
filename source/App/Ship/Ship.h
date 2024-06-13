#pragma once

#include <wv/Math/Transform.h>

namespace wv { class Mesh; }
namespace wv { class GraphicsDevice; }

class Ship
{
public:

	 Ship( wv::Mesh* _mesh );
	~Ship();

	virtual void update( double _deltaTime );
	virtual void draw( wv::GraphicsDevice* _device );

	wv::Transformf& getTransform() { return m_transform; }
	wv::Transformf getPreviousTransform() { return m_previousTransform; }

	void setMaxSpeed( float _speed ) { m_maxSpeed = _speed; }
	void setTargetRotation( const wv::Vector3f& _target ) { m_targetRotation = _target; }

	wv::Vector3f getLastVelocity() { return m_previousTransform.position - m_transform.position; }

	void kill() { m_dead = true; }

protected:

	bool m_dead = false;

	wv::Transformf m_transform{};
	wv::Vector3f m_targetRotation;
	wv::Transformf m_previousTransform{};


	float m_throttle = 1.0f;
	float m_maxSpeed = 10.0f;
	float m_health   = 100.0f;

	float radius = 1.0f;
	float roll = 0.0f;

	wv::Mesh* m_mesh = nullptr;
};