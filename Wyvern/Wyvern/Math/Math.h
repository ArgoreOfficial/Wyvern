#include <Wyvern/Core/defs.h>

namespace wv
{
	namespace Math
	{
		template<typename T>
		T clamp( const T& _value, const T& _min, const T& _max ) { return T; }
		
		template<>
		float clamp( const float& _value, const float& _min, const float& _max )
		{
			const float t = _value < _min ? _min : _value;
			return t > _max ? _max : t;
		}

		template<>
		double clamp( const double& _value, const double& _min, const double& _max )
		{
			const double t = _value < _min ? _min : _value;
			return t > _max ? _max : t;
		}

		template<>
		int clamp( const int& _value, const int& _min, const int& _max )
		{
			const int t = _value < _min ? _min : _value;
			return t > _max ? _max : t;
		}
	}
}