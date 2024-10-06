#pragma once

#include <wv/Camera/Camera.h>

#include <wv/Events/Events.h>

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class OrbitCamera : public iCamera
	{
	public:

		OrbitCamera( CameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 10000.0f );

		~OrbitCamera( void );

		void onCreate() override;

		void handleInput();
		void update( double _delta_time ) override;

///////////////////////////////////////////////////////////////////////////////////////

	private:

		cMouseEventListener m_mouseListener;
		cInputEventListener m_inputListener;

		wv::Vector2f m_rotate;

		bool m_input_enabled = false;
		float r = 0.0f;

	};

}

