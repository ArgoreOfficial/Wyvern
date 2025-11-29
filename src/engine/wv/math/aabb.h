#pragma once

#include "vector2.h"

namespace wv {

template<typename Ty>
struct AABB2D
{
	wv::Vector2<Ty> center, extent;

	wv::Vector2<Ty> getMin() const { return center - extent; }
	wv::Vector2<Ty> getMax() const { return center + extent; }

	bool contains( const AABB2D& _other );
	bool contains( const wv::Vector2<Ty>& _point );

};

typedef AABB2D<int32_t> AABB2Di;
typedef AABB2D<float>   AABB2Df;
typedef AABB2D<double>  AABB2Dd;

template<typename Ty>
inline bool AABB2D<Ty>::contains( const AABB2D& _other )
{
	return ( wv::Math::abs( center.x - _other.center.x ) * 2 < ( extent.x + _other.extent.x ) ) &&
		   ( wv::Math::abs( center.y - _other.center.y ) * 2 < ( extent.y + _other.extent.y ) );
}

template<typename Ty>
inline bool AABB2D<Ty>::contains( const wv::Vector2<Ty>& _point )
{
	const wv::Vector2f min = getMin();
	const wv::Vector2f max = getMax();

	return
		_point.x >= min.x &&
		_point.x <= max.x &&
		_point.y <= min.y &&
		_point.y >= max.y;
}

}