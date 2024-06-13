#pragma once

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

	namespace Math
	{

	#define WV_CONSTF_PI 3.141592653f
	#define WV_CONST_PI  3.141592653

///////////////////////////////////////////////////////////////////////////////////////

		template <typename T> T clamp( const T& _value, const T& _min, const T& _max );
		template <typename T> T pow2 ( const T& _value );
		template <typename T> T pow3 ( const T& _value );
		template <typename T> T pow4 ( const T& _value );
		template <typename T> T pow5 ( const T& _value );
		template <typename T> T abs  ( const T& _value );
		template <typename T> T min  ( const T& _a, const T& _b );
		template <typename T> T max  ( const T& _a, const T& _b );
		template <typename T> T lerp ( const T& _a, const T& _b, const T& _t );

		inline float  degToRad( const float&  _degrees ) { return _degrees * ( WV_CONSTF_PI / 180.0f ); }
		inline double degToRad( const double& _degrees ) { return _degrees * ( WV_CONST_PI  / 180.0 ); }

		inline float  radToDeg( const float&  _radians ) { return _radians * ( 180.0f / WV_CONSTF_PI ); }
		inline double radToDeg( const double& _radians ) { return _radians * ( 180.0  / WV_CONST_PI ); }


///////////////////////////////////////////////////////////////////////////////////////

		template < typename T > T clamp( const T& _value, const T& _min, const T& _max )
		{ 
			const T t = _value < _min ? _min : _value;
			return t > _max ? _max : t;
		}

		template< typename T > T pow2( const T& _value )
		{
			return _value * _value;
		}

		template< typename T > T pow3( const T& _value )
		{
			return _value * _value * _value;
		}

		template< typename T > T pow4( const T& _value )
		{
			return _value * _value * _value * _value;
		}

		template< typename T > T pow5( const T& _value )
		{
			return _value * _value * _value * _value * _value;
		}

		template< typename T > T abs( const T& _value )
		{
			return _value < T( 0 ) ? -_value : _value;
		}

		template< typename T > T min( const T& _a, const T& _b )
		{
			return _a < _b ? _a : _b;
		}

		template< typename T > T max( const T& _a, const T& _b )
		{
			return _a > _b ? _a : _b;
		}

		template<typename T>
		T lerp( const T& _a, const T& _b, const T& _t )
		{
			return _a + _t * ( _b - _a );
		}

///////////////////////////////////////////////////////////////////////////////////////

	}

}