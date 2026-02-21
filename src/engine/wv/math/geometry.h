#pragma once

#include <wv/math/vector3.h>

namespace wv {
namespace Math {

bool linePlaneIntersection( const Vector3f& _rayOrigin, const Vector3f& _rayDir, const Vector3f& _planeOrigin, const Vector3f& _planeNormal, Vector3f* _outPoint )
{
	if ( _planeNormal.dot( _rayDir ) == 0 ) 
		return false; // ray is parallel, intersection is impossible

	const float d = _planeNormal.dot( _planeOrigin );
	const float x = ( d - _planeNormal.dot( _rayOrigin ) ) / _planeNormal.dot( _rayDir );

	if ( x < 0.0f )
		return false; // hit point is behind ray

	*_outPoint = _rayOrigin + _rayDir.normalized() * x;

	return true;
}

}
}