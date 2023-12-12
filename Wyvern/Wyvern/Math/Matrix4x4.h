#pragma once

#include <Wyvern/Math/Vector3.h>
#include <Wyvern/Math/Vector4.h>

namespace wv
{

	template<typename T>
	class cMatrix4x4
	{
	public:
		
		 cMatrix4x4( void ) { }
		 cMatrix4x4( cVector4<T> _right, cVector4<T> _up, cVector4<T> _forward ) : 
			 m_data( _right, _up, _forward ) { }

		~cMatrix4x4( void ) { }

	private:

		union uMat4Data
		{
			struct
			{
				cVector4<T> right;
				cVector4<T> up;
				cVector4<T> forward;
			};

			T data[ 16 ];
		};

		uMat4Data m_data;

	};

	namespace Matrix4x4
	{
		
		template<typename T>
		cMatrix4x4<T> translate( cVector3<T> _move )
		{

		}

	}


}