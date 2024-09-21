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

	class OrbitCamera : public iCamera, public iMouseListener, public iInputListener
	{
	public:

		OrbitCamera( CameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 10000.0f );

		~OrbitCamera( void );

		void onCreate() override;
		void onMouseEvent( MouseEvent _event ) override;
		void onInputEvent( sInputEvent _event ) override;
		void update( double _delta_time ) override;

///////////////////////////////////////////////////////////////////////////////////////

	private:

		wv::Vector2f m_rotate;

		wv::Vector2i m_old_mouse_pos;
		bool m_input_enabled = false;

		float r = 0.0f;

	};

}

