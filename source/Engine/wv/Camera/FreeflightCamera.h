#pragma once

#include <wv/Camera/ICamera.h>

#include <wv/Events/IMouseListener.h>
#include <wv/Events/IInputListener.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

namespace wv
{
	
	class FreeflightCamera : public ICamera, public IMouseListener, public IInputListener
	{
	public:

		FreeflightCamera( CameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 100.0f );

		~FreeflightCamera( void );

		void onCreate() override;
		void onMouseEvent( MouseEvent _event ) override;
		void onInputEvent( InputEvent _event ) override;
		void update    ( double _delta_time ) override;
	
	private:

		wv::Vector3f m_move;
		wv::Vector2f m_rotate;
		wv::Vector3f m_velocity;

		wv::Vector2i m_old_mouse_pos;
		bool m_freecam_enabled = false;

		float m_speed = 50.0f;
		float m_speed_normal = 50.0f;
		
	};

}

