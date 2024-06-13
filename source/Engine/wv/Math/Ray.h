#pragma once

#include <wv/Math/Triangle.h>
#include <wv/Primitive/Mesh.h>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct RayIntersection
	{
		bool hit = false;
		Vector3f point;
		Vector3f tuvValue;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class Ray
	{
	public:

		Ray( void )                                         : start{ },        end{ }      { }
		Ray( const Vector3f& _start, const Vector3f& _end ) : start{ _start }, end{ _end } { }

		Vector3f start;
		Vector3f end;

		template<typename T>
		RayIntersection intersect( T* _t );

		float length() { return ( end - start ).length(); }
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline RayIntersection Ray::intersect( T* _t ) { return {}; }

	// https://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/raytri_tam.pdf
	template<>
	inline RayIntersection Ray::intersect<Triangle3f>( Triangle3f* _t )
	{
		if ( start == end )
			return {};

		Vector3f dir = end - start;
		dir.normalize();

		////////////////////////////////////////////

		float u, v, t;

		Vector3f edge1 = _t->edge1();
		Vector3f edge2 = _t->edge2();
		Vector3f pvec = dir.cross( edge2 );
		float det = edge1.dot( pvec );

		#ifndef TEST_CULL
		if ( det < wv::Const::Float::EPSILON )
			return {};

		Vector3f tvec = start - _t->v0;

		u = tvec.dot( pvec );
		if ( u < 0.0f || u > det )
			return {};

		Vector3f qvec = tvec.cross( edge1 );

		v = dir.dot( qvec );
		if ( v < 0.0f || u + v > det )
			return {};

		t = edge2.dot( qvec );
		float invDet = 1.0f / det;
		t *= invDet;
		u *= invDet;
		v *= invDet;
		
		#else
		if ( det > -wv::Const::Float::EPSILON && det < wv::Const::Float::EPSILON )
			return {};
		float invDet = 1.0f / det;
		
		Vector3f tvec = start - _t->v0;
		u = tvec.dot( pvec ) * invDet;
		if ( u < 0.0f || u > 1.0f )
			return {};

		Vector3f qvec = tvec.cross( edge1 );
		v = dir.dot( qvec ) * invDet;
		if ( v < 0.0f || u + v > 1.0f )
			return {};

		t = edge2.dot( qvec ) * invDet;
		#endif

		RayIntersection result;
		result.tuvValue = { t, u, v };
		result.hit = true;
		result.point = _t->barycentricToCartesian( u, v );

		return result;
	}

	template<>
	inline RayIntersection Ray::intersect<Mesh>( Mesh* _t )
	{
		/// TODO: remove glm

		glm::vec4 s = glm::vec4{ start.x, start.y, start.z, 1.0f }; // * _t->transform.getMatrix();
		glm::vec4 e = glm::vec4{   end.x,   end.y,   end.z, 1.0f }; // * _t->transform.getMatrix();
		
		wv::Ray ray{
			wv::Vector3f{ s.x, s.y, s.z },
			wv::Vector3f{ e.x, e.y, e.z }
		};

		RayIntersection finalResult;
		for ( int i = 0; i < _t->triangles.size(); i++ )
		{
			wv::RayIntersection result = ray.intersect( &_t->triangles[ i ] );
			if ( result.hit )
			{
				if ( result.tuvValue.x < 0.0 )
					continue;

				if ( !finalResult.hit || result.tuvValue.x < finalResult.tuvValue.x )
					finalResult = result;
			}
		}

		return finalResult;
	}

}