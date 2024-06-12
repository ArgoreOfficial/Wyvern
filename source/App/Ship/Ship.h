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
	void draw( wv::GraphicsDevice* _device );

	wv::Transformf& getTransform() { return m_transform; }

protected:

	wv::Transformf m_transform{};

	float m_throttle = 1.0f;
	float m_maxSpeed = 10.0f;
	float m_health   = 100.0f;

	float radius = 1.0f;

	wv::Mesh* m_mesh;
};