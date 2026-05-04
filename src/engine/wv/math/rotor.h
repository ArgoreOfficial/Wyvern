#pragma once

#include <wv/math/vector3.h>
#include <wv/math/vector4.h>
#include <wv/math/matrix.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

///////////////////////////////////////////////////////////////////////////////////////

namespace Math {

template<typename Ty>
inline Vector3<Ty> wedge( const Vector3<Ty>& u, const Vector3<Ty>& v )
{
	return {
		u.x * v.y - u.y * v.x, // XY
		u.x * v.z - u.z * v.x, // XZ
		u.y * v.z - u.z * v.y  // YZ
	};
}

}

///////////////////////////////////////////////////////////////////////////////////////

enum RotateSpace
{
	RotateSpace_local,
	RotateSpace_world
};

template<typename Ty>
class Rotor
{
public:
	Ty s{ 1 };
	Ty xy{ 0 };
	Ty xz{ 0 };
	Ty yz{ 0 };

	Rotor() = default;
	
	Rotor( Ty _s, Ty _xy, Ty _xz, Ty _yz ) :
		s{ _s },
		xy{ _xy },
		xz{ _xz },
		yz{ _yz }
	{ }

	Rotor( Ty _s, const Vector3<Ty>& _bivector ) :
		s{ _s },
		xy{ _bivector.x },
		xz{ _bivector.y },
		yz{ _bivector.z }
	{ }

	Rotor( const Vector3<Ty>& _bvPlane, Ty _radians )
	{ 
		const Ty sina = std::sin( _radians / 2.0 );
		s = std::cos( _radians / 2.0 );
		xy = -sina * _bvPlane.x;
		xz = -sina * _bvPlane.y;
		yz = -sina * _bvPlane.z;
	}

	Rotor( const Rotor<Ty>& _other ) :
		s{ _other.s },
		xy{ _other.xy },
		xz{ _other.xz },
		yz{ _other.yz }
	{ }

	Ty length() const;
	Rotor<Ty> reverse() const;

	void rotate( const Rotor<Ty>& _r,       RotateSpace _space = RotateSpace_local );
	void rotate( const Vector3<Ty>& _euler, RotateSpace _space = RotateSpace_local );
	
	inline Vector3<Ty> rotateVector( const Vector3<Ty>& _v ) const; 
	inline Matrix<Ty, 4, 4> toMatrix4x4() const;

	Rotor<Ty> normalized() const {
		Rotor<Ty> r = *this;
		Ty magnitude = length();
		r.xy /= magnitude;
		r.xz /= magnitude;
		r.yz /= magnitude;
		return r;
	}

	// I don't know how either of these work

	inline Vector4<Ty> toQuaternion() const {
		return { 
			-yz, 
			 xz, 
			-xy,
			  s 
		};
	}

	inline Vector3<Ty> toEuler() const {
		const Ty w =  s;
		const Ty x = -yz;
		const Ty y =  xz;
		const Ty z = -xy;

		const Vector3<Ty> euler2{
			(Ty)std::asin ( 2.0 * ( w * x - y * z ) ),
			(Ty)std::atan2( 2.0 * ( w * y + x * z ), 1.0 - 2.0 * ( x * x + y * y ) ),
			(Ty)std::atan2( 2.0 * ( w * z + x * y ), 1.0 - 2.0 * ( x * x + z * z ) )
		};

		return {
			Math::degrees( euler2.x ),
			Math::degrees( euler2.y ),
			Math::degrees( euler2.z )
		};
	}

	static Rotor<Ty> fromQuaternion( const Vector4<Ty>& _q ) {
		return {
			 _q.w, //
			-_q.z, // xy
			 _q.y, // xz
			-_q.x  // yz
		};
	}

	static Rotor<Ty> euler( const Ty& _x, const Ty& _y, const Ty& _z );
	static Rotor<Ty> euler( const Vector3<Ty>& _degrees );

///////////////////////////////////////////////////////////////////////////////////////

	Rotor<Ty> operator*( const Rotor<Ty>& _r ) const;

	bool operator ==( const Rotor<Ty>& _other ) const;
	bool operator !=( const Rotor<Ty>& _other ) const { return !( *this == _other ); }

///////////////////////////////////////////////////////////////////////////////////////

};

///////////////////////////////////////////////////////////////////////////////////////

typedef Rotor<float> Rotorf;
typedef Rotor<double> Rotord;

///////////////////////////////////////////////////////////////////////////////////////

template<typename Ty>
inline Ty Rotor<Ty>::length() const
{
	return std::sqrt( s * s + xy * xy + xz * xz + yz * yz );
}

template<typename Ty>
inline Rotor<Ty> Rotor<Ty>::reverse() const
{
	return { s, -xy, -xz, -yz };
}

template<typename Ty>
inline void Rotor<Ty>::rotate( const Rotor<Ty>& _r, RotateSpace _space )
{
	Rotor<Ty>& r = *this;
	if( _space == RotateSpace_local )
		r = r * _r;
	else
		r = _r * r;
}

template<typename Ty>
inline void Rotor<Ty>::rotate( const Vector3<Ty>& _euler, RotateSpace _space )
{
	rotate( Rotor<Ty>::euler( _euler.x, _euler.y, _euler.z ), _space );
}

template<typename Ty>
inline Vector3<Ty> Rotor<Ty>::rotateVector( const Vector3<Ty>& _v ) const
{
	const Rotor<Ty>& p = *this;

	// q = P x
	Vector3<Ty> q{
		p.s * _v.x + _v.y * p.xy + _v.z * p.xz,
		p.s * _v.y - _v.x * p.xy + _v.z * p.yz,
		p.s * _v.z - _v.x * p.xz - _v.y * p.yz
	};

	Ty q012 = _v.x * p.yz - _v.y * p.xz + _v.z * p.xy; // trivector

	// trivector part of the result is always zero

	// r = q P*
	return {
		p.s * q.x + q.y * p.xy + q.z * p.xz + q012 * p.yz,
		p.s * q.y - q.x * p.xy - q012 * p.xz + q.z * p.yz,
		p.s * q.z + q012 * p.xy - q.x * p.xz - q.y * p.yz
	};
}

template<typename Ty>
inline Matrix<Ty, 4, 4> Rotor<Ty>::toMatrix4x4() const
{
	Vector3<Ty> v0 = rotateVector( Vector3<Ty>{ 1, 0, 0 } );
	Vector3<Ty> v1 = rotateVector( Vector3<Ty>{ 0, 1, 0 } );
	Vector3<Ty> v2 = rotateVector( Vector3<Ty>{ 0, 0, 1 } );

	Matrix<Ty, 4, 4> m{};
	m.setRow( 0, { v0.x, v0.y, v0.z, 0 } );
	m.setRow( 1, { v1.x, v1.y, v1.z, 0 } );
	m.setRow( 2, { v2.x, v2.y, v2.z, 0 } );
	m.setRow( 3, { 0,    0,    0, 1 } );
	return m;
}

template<typename Ty>
Rotor<Ty> Rotor<Ty>::euler( const Ty& _x, const Ty& _y, const Ty& _z )
{
	Rotor<Ty> xrotor( { 0.0,  0.0, 1.0 }, wv::Math::radians( (double)_x ) );
	Rotor<Ty> yrotor( { 0.0, -1.0, 0.0 }, wv::Math::radians( (double)_y ) );
	Rotor<Ty> zrotor( { 1.0,  0.0, 0.0 }, wv::Math::radians( (double)_z ) );

	Rotor<Ty> r;
	r = r * yrotor;
	r = r * xrotor;
	r = r * zrotor;
	return r;
}

template<typename Ty>
Rotor<Ty> Rotor<Ty>::euler( const Vector3<Ty>& _degrees )
{
	return Rotor<Ty>::euler( _degrees.x, _degrees.y, _degrees.z );
}

template<typename Ty>
inline Rotor<Ty> Rotor<Ty>::operator*( const Rotor<Ty>& _r ) const
{
	const Rotor<Ty>& p = *this;
	Rotor<Ty> r;

	// here we just expanded the geometric product rules

	r.s = p.s * _r.s
		- p.xy * _r.xy
		- p.xz * _r.xz
		- p.yz * _r.yz;

	r.xy = p.xy * _r.s
		+ p.s * _r.xy
		+ p.yz * _r.xz
		- p.xz * _r.yz;

	r.xz = p.xz * _r.s
		+ p.s * _r.xz
		- p.yz * _r.xy
		+ p.xy * _r.yz;

	r.yz = p.yz * _r.s
		+ p.s * _r.yz
		+ p.xz * _r.xy
		- p.xy * _r.xz;

	return r;
}

template<typename Ty>
inline bool Rotor<Ty>::operator==( const Rotor<Ty>& _other ) const
{
	return  s == _other.s
		&& xy == _other.xy
		&& xz == _other.xz
		&& yz == _other.yz;
}

namespace Math {

template<typename Ty>
Rotor<Ty> lerp( const Rotor<Ty>& _a, const Rotor<Ty>& _b, float _t )
{
	Rotor<Ty> r{};
	r.s  = wv::Math::lerp( _a.s,  _b.s,  _t );
	r.xy = wv::Math::lerp( _a.xy, _b.xy, _t );
	r.xz = wv::Math::lerp( _a.xz, _b.xz, _t );
	r.yz = wv::Math::lerp( _a.yz, _b.yz, _t );

	return r.normalized();
}

}

}