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

wv::Vector3f wv::ViewVolume::screenToWorld( int _pixelX, int _pixelY, float _depth )
{
	float clipX = static_cast<float>( _pixelX ) / m_viewDimensions.x;
	float clipY = static_cast<float>( _pixelY ) / m_viewDimensions.y;
	
	clipX = clipX * 2.0f - 1.0f;
	clipY = clipY * 2.0f - 1.0f;

	return screenToWorld( clipX, clipY, _depth );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Vector3f wv::ViewVolume::screenToWorld( float _clipX, float _clipY, float _depth )
{
	Matrix4x4f invViewProj = m_viewProjMatrix.inverse();
	Vector4f screenspacePoint{ _clipX, _clipY, -_depth, 1.0f };
	Vector4f worldPoint = screenspacePoint * invViewProj;

	return Vector3f{ 
		worldPoint.x / worldPoint.w, 
		worldPoint.y / worldPoint.w, 
		worldPoint.z / worldPoint.w 
	};
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ViewVolume::calculatePerspectiveMatrix( void ) const
{
	return Math::perspective( m_aspect, Math::radians( fov ), m_near, m_far );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::Matrix4x4f wv::ViewVolume::calculateOrthographicMatrix( void ) const
{
	return Math::orthographic( 
		m_viewDimensions.x * m_orthoScale / 2.0f, 
		m_viewDimensions.y * m_orthoScale / 2.0f, 
		-1000.0f, 
		 1000.0f );
}
