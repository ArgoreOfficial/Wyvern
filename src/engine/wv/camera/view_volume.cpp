#include "view_volume.h"

///////////////////////////////////////////////////////////////////////////////////////

wv::ViewVolume::ViewVolume( CameraType _type, size_t _width, size_t _height, float _fov, float _near, float _far ) :
	fov   { _fov  },
	m_near{ _near },
	m_far { _far  },
	m_type{ _type }
{
	setViewDimensions( _width, _height );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::ViewVolume::setViewDimensions( size_t _width, size_t _height )
{
	m_viewDimensions.x = _width;
	m_viewDimensions.y = _height;

	m_aspect = m_viewDimensions.x / m_viewDimensions.y;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::ViewVolume::setViewDimensions( size_t _width, float _aspect )
{
	m_viewDimensions.x = _width;
	m_viewDimensions.y = _width / _aspect;

	m_aspect = _aspect;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::ViewVolume::screenToWorld( int _pixelX, int _pixelY, float _depth )
{
	float clipX = static_cast<float>( _pixelX ) / m_viewDimensions.x;
	float clipY = static_cast<float>( _pixelY ) / m_viewDimensions.y;
	
	clipX = clipX * 2.0f - 1.0f;
	clipY = clipY * 2.0f - 1.0f;

	return screenToWorld( clipX, -clipY, _depth );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::ViewVolume::screenToWorld( float _clipX, float _clipY, float _depth )
{
	Matrix4x4f viewProj = getViewMatrix() * getProjectionMatrix();
	Matrix4x4f invViewProj = viewProj.inverse();
	
	Vector4f screenspacePoint{ _clipX, _clipY, -_depth, 1.0f };
	Vector4f worldPoint = screenspacePoint * invViewProj;

	return Vector3f{ 
		worldPoint.x / worldPoint.w, 
		worldPoint.y / worldPoint.w, 
		worldPoint.z / worldPoint.w 
	};
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ViewVolume::getProjectionMatrix( void ) const
{
	switch( m_type )
	{
	case kPerspective:  return getPerspectiveMatrix();      break;
	case kOrthographic: return getOrthographicMatrix();     break;
	}

	return Matrix4x4f{ 1.0f };
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ViewVolume::getPerspectiveMatrix( void ) const
{
	return Math::perspective( m_aspect, Math::radians( fov ), m_near, m_far );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ViewVolume::getOrthographicMatrix( void ) const
{
	return Math::orthographic( m_ortho_width / 2.0f, m_ortho_height / 2.0f, -1000.0f, 1000.0f );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ViewVolume::getViewMatrix( void ) const
{
	return m_transform.getMatrix().inverse();
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::ViewVolume::getViewDirection() const
{
	float yaw   = Math::radians( m_transform.rotation.y - 90.0f );
	float pitch = Math::radians( m_transform.rotation.x );

	Vector3f direction;
	direction.x = std::cos( yaw ) * std::cos( pitch );
	direction.y = std::sin( pitch );
	direction.z = std::sin( yaw ) * std::cos( pitch );
    return direction;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::ViewVolume::setOrthoWidth( float _width )
{
	m_ortho_width  = _width;
	m_ortho_height = _width / m_aspect;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::ViewVolume::setOrthoHeight( float _height )
{
	m_ortho_height = _height;
	m_ortho_width  = _height * m_aspect;
}
