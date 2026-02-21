#pragma once

#include <wv/math/math.h>
#include <wv/math/vector3.h>

namespace wv {
namespace Math {

static bool linePlaneIntersection( const Vector3f& _rayOrigin, const Vector3f& _rayDir, const Vector3f& _planeOrigin, const Vector3f& _planeNormal, Vector3f* _outPoint )
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

static wv::Vector3f lineClosestPointClamped( const Vector3f& _p0, const Vector3f& _p1, const wv::Vector3f& _point )
{
	Vector3f d = _p1 - _p0;
	const double lineLength = d.length();

	d.normalize();

	const Vector3f v = _point - _p0;
	double dotProd = v.dot( d );

	dotProd = wv::Math::clamp( dotProd, 0.0, lineLength );

	return _p0 + d * dotProd;
}

}
}