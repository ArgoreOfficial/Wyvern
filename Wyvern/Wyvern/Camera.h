#pragma once

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/math.h>

namespace WV
{
	class Camera
	{
	public:
		Camera();
		~Camera();

		void setPosition( float _x, float _y, float _z ) { m_position = { _x, _y, _z }; }
		void setRotation( float _x, float _y, float _z  ) { m_rotation = { _x, _y, _z }; };

		void move( float _x, float _y, float _z ) 
		{ 
			m_position.x += _x;
			m_position.y += _y;
			m_position.z += _z;
		}

		void rotate( float _x, float _y, float _z )
		{
			m_rotation.x += _x;
			m_rotation.y += _y;
			m_rotation.z += _z;
		}

		void setFOV( float _fov ) { m_fov = _fov; }
		void setAspect( float _aspect ) { m_aspect = _aspect; }

		void submit();

		bx::Vec3 m_position = {0,0,0};
		bx::Vec3 m_rotation = {0,0,0};
	private:
		float m_view[ 16 ];
		float m_proj[ 16 ];


		float m_aspect;
		float m_fov;
	};
}