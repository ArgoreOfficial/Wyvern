#pragma once

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	struct sGraphPoint
	{
		T x, y;
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	struct sLinearEquation
	{
		T min, max;
		T m, b;

		sLinearEquation() : m{0}, b{0}, min{0}, max{0} { }
		sLinearEquation( const sGraphPoint<T>& _a, const sGraphPoint<T>& _b ):
			m  { ( _b.y - _a.y ) / ( _b.x - _a.x ) },
			b  { _a.y },
			min{ _a.x },
			max{ _b.x }
		{ }

		// remember, y = mx + b
		T valueAt( const T& _x ) { return m * ( _x - min ) + b; }
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	class cLinearGraph
	{
	public:

		cLinearGraph( const std::vector<sGraphPoint<T>>& _points );

		T getValue( const T& _x );
		sLinearEquation<T> getLinearEquation( const T& _x );

	private:

		std::vector<sLinearEquation<T>> m_linearEquations;

	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline wv::cLinearGraph<T>::cLinearGraph( const std::vector<sGraphPoint<T>>& _points )
	{
		m_linearEquations.resize( _points.size() );
		
		for ( size_t i = 0; i < _points.size(); i++ )
		{
			if ( i >= _points.size() - 1 )
				continue;

			m_linearEquations[ i ] = sLinearEquation<T>( _points[ i ], _points[ i + 1 ] );
		}
	}

	template<typename T>
	inline T wv::cLinearGraph<T>::getValue( const T& _x )
	{
		for ( size_t i = 0; i < m_linearEquations.size(); i++ )
		{
			sLinearEquation<T>& line = m_linearEquations[ i ];

			if ( _x < line.min || _x >= line.max )
				continue;

			return line.valueAt( _x );
		}
		
		return T{ 0 };
	}

	template<typename T>
	inline sLinearEquation<T> cLinearGraph<T>::getLinearEquation( const T& _x )
	{
		for ( size_t i = 0; i < m_linearEquations.size(); i++ )
		{
			sLinearEquation<T>& line = m_linearEquations[ i ];

			if ( _x < line.min || _x >= line.max )
				continue;

			return line;
		}

		return sLinearEquation<T>{};
	}

}

