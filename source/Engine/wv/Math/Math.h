#pragma once

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{
	namespace Const /// TODO: move
	{
		namespace Float
		{
			constexpr float PI = 3.141592f;
			constexpr float EPSILON = 0.00000001f;
		}

		namespace Double
		{
			constexpr float PI = 3.141592;
			constexpr float EPSILON = 0.000001;
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

	namespace Math
	{
		
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

		inline float  radians( const float&  _degrees ) { return _degrees * ( Const::Float::PI  / 180.0f ); }
		inline double radians( const double& _degrees ) { return _degrees * ( Const::Double::PI / 180.0 ); }

		inline float  degrees( const float&  _radians ) { return _radians * ( 180.0f / Const::Float::PI ); }
		inline double degrees( const double& _radians ) { return _radians * ( 180.0  / Const::Double::PI ); }


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