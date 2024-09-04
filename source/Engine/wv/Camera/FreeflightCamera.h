#pragma once

#include <wv/Camera/Camera.h>

#include <wv/Events/MouseListener.h>
#include <wv/Events/InputListener.h>

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class FreeflightCamera : public ICamera, public IMouseListener, public IInputListener
	{

	public:

		FreeflightCamera( CameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 10000.0f );

		~FreeflightCamera() {}

		void onCreate() override;
		void onMouseEvent( MouseEvent _event ) override;
		void onInputEvent( InputEvent _event ) override;
		void update( double _delta_time ) override;
	
		void resetVelocity() { m_velocity = { 0.0f, 0.0f, 0.0f }; }

///////////////////////////////////////////////////////////////////////////////////////

	private:

		wv::cVector3f m_move;
		wv::Vector2f m_rotate;
		wv::cVector3f m_velocity;

		bool m_freecam_enabled = false;

		float m_speed = 70.0f;
		float m_speed_normal = 70.0f;
		
	};

}

