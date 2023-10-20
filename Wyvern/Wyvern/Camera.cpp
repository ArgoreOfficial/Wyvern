#include "Camera.h"
#include <tgmath.h>

using namespace WV;

Camera::Camera()
{

}

WV::Camera::~Camera()
{

}

void WV::Camera::submit()
{
	bx::Vec3 dir = {
		sinf( m_rotation.y ),
		-( sin( m_rotation.x ) * cos( m_rotation.y ) ),
		-( cos( m_rotation.x ) * cos( m_rotation.y ) )
	};

	bx::Vec3 at = {
		m_position.x + dir.x,
		m_position.y + dir.y,
		m_position.z + dir.z
	};

	bx::mtxLookAt( m_view, m_position, at );
	bx::mtxProj( m_proj, m_fov, m_aspect, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth );

	bgfx::setViewTransform( 0, m_view, m_proj );

}
