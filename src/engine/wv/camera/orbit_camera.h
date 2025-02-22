#pragma once

#include <wv/camera/camera.h>

#include <wv/event/events.h>

#include <wv/math/transform.h>
#include <wv/math/vector3.h>
#include <wv/math/vector2.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class OrbitCamera : public ICamera
	{
	public:

		OrbitCamera( CameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 10000.0f );

		~OrbitCamera( void );

		void onEnter() override;

		void handleInput();
		void update( double _delta_time ) override;

///////////////////////////////////////////////////////////////////////////////////////

	private:

		MouseEventListener m_mouseListener;
		InputEventListener m_inputListener;

		wv::Vector2f m_rotate;

		bool m_input_enabled = false;
		float r = 0.0f;

	};

}

