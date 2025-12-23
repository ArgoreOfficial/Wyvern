#include "orbit_camera_component.h"

#include <wv/camera/view_volume.h>
#include <wv/memory/memory.h>

wv::OrbitCameraComponent::OrbitCameraComponent()
{
	m_camera = WV_NEW( ViewVolume, ViewVolume::kPerspective, 900, 600 );
}

wv::OrbitCameraComponent::~OrbitCameraComponent()
{
	if ( m_camera ) 
		WV_FREE( m_camera );
}
