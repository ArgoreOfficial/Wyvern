#pragma once

#include <wv/Camera/Camera.h>

#include <wv/event/events.h>

#include <wv/math/transform.h>
#include <wv/math/vector3.h>
#include <wv/math/vector2.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class FreeflightCamera : public ICamera
	{

	public:

		FreeflightCamera( CameraType _type, float _fov = 60.0f, float _near = 0.1f, float _far = 500.0f );

		~FreeflightCamera() {}

		void onEnter() override;

		void handleInput();
		void update( double _delta_time ) override;
	
		void resetVelocity() { m_velocity = { 0.0f, 0.0f, 0.0f }; }

///////////////////////////////////////////////////////////////////////////////////////

	private:

		MouseEventListener m_mouseListener;
		InputEventListener m_inputListener;

		wv::Vector3f m_move;
		wv::Vector2f m_rotate;
		wv::Vector3f m_velocity;

		bool m_freecam_enabled = false;

		float m_speed = 100.0f;
		float m_speed_normal = 100.0f;
		
	};

}

