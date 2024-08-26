#pragma once

#include <wv/Math/Vector3.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	class Triangle
	{

	public:

		cVector3<T> v0, v1, v2;

		Triangle( void ) : v0{}, v1{}, v2{} {}
		Triangle( const cVector3<T>& _a, 
				  const cVector3<T>& _b, 
				  const cVector3<T>& _c ) 
			: v0{ _a }, v1{ _b }, v2{ _c } {}

		cVector3<T> edge1() const { return v1 - v0; }
		cVector3<T> edge2() const { return v2 - v0; }

		cVector3<T> getNormal() const{ return edge1().cross( edge2() ).normalized(); }

		cVector3<T> barycentricToCartesian( const T& _u, const T& _v ) const;

	};

///////////////////////////////////////////////////////////////////////////////////////

	typedef Triangle<float> Triangle3f;
	typedef Triangle<double> Triangle3d;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline cVector3<T> Triangle<T>::barycentricToCartesian( const T& _u, const T& _v ) const
	{
		// https://alexpolt.github.io/barycentric.html
		return v0 + edge1() * _u + 
			        edge2() * _v;
	}

}