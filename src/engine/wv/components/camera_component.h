#pragma once

#include <wv/math/transform.h>
#include <wv/math/vector3.h>
#include <wv/math/vector2.h>

#include <wv/decl.h>

namespace wv {

class ViewVolume;

struct CameraComponent
{
	enum CameraType
	{
		kPerspective,
		kOrthographic
	};

	float getAspect() { return viewDimensions.x / viewDimensions.y; }

	inline void recalculateViewMatrix( Transform* _parent, bool _cacheViewProj = true )
	{
		if ( _parent )
			viewMatrix = _parent->getMatrix().inverse();
		else
			viewMatrix = wv::Matrix4x4f::identity( 1.0f ).inverse();

		if ( _cacheViewProj )
			viewProjMatrix = viewMatrix * projMatrix;
	}

	inline void recalculateProjMatrix( bool _cacheViewProj = true ) {
		switch ( projectionType )
		{
		case kPerspective:  
			projMatrix = Math::perspective( getAspect(), Math::radians(fov), clipNear, clipFar);
			break;

		case kOrthographic: 
			projMatrix = Math::orthographic(
				viewDimensions.x * orthoScale / 2.0f,
				viewDimensions.y * orthoScale / 2.0f,
				-1000.0f,
				1000.0f );
			break;
		}

		if ( _cacheViewProj )
			viewProjMatrix = viewMatrix * projMatrix;
	}

	Vector3f screenToWorld( int _pixelX, int _pixelY, float _depth ) {
		float clipX = static_cast<float>( _pixelX ) / viewDimensions.x;
		float clipY = static_cast<float>( _pixelY ) / viewDimensions.y;

		clipX = clipX * 2.0f - 1.0f;
		clipY = clipY * 2.0f - 1.0f;

		return screenToWorld( clipX, clipY, _depth );
	}

	Vector3f screenToWorld( float _clipX, float _clipY, float _depth ) {
		Matrix4x4f invViewProj = viewProjMatrix.inverse();

		_depth = wv::Math::clamp( _depth, 0.0f, 1.0f );

		Vector4f screenspacePoint{ _clipX, _clipY, 1.0f - _depth, 1.0f };
		Vector4f worldPoint = screenspacePoint * invViewProj;

		return Vector3f{
			worldPoint.x / worldPoint.w,
			worldPoint.y / worldPoint.w,
			worldPoint.z / worldPoint.w
		};
	}

///////////////////////////////////////////////////////////////////////////////////////

	bool active;
	CameraType projectionType = kPerspective;
	
	float fov        = 60.0f;
	float clipNear   = 0.01f;
	float clipFar    = 10000.0f;
	float orthoScale = 0.1f;
	float aspect     = 1.777777778f;

	wv::Vector2f viewDimensions{ 900.0f, 600.0f };

	Matrix4x4f viewMatrix{};
	Matrix4x4f projMatrix{};
	Matrix4x4f viewProjMatrix{};
};

}