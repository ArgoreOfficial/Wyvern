#pragma once

#include <wv/Camera/iCamera.h>

#include <wv/Events/iMouseListener.h>
#include <wv/Events/iInputListener.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

namespace wv
{
	
	class cFreeflightCamera : public iCamera, public iMouseListener, public iInputListener
	{
	public:

		cFreeflightCamera( eCameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 100.0f );

		~cFreeflightCamera( void );

		void onCreate() override;
		void onMouseEvent( sMouseEvent _event ) override;
		void onInputEvent( sInputEvent _event ) override;
		void update    ( double _delta_time ) override;
	
	private:

		wv::cVector3f m_move;
		wv::cVector2f m_rotate;

		wv::cVector2i m_old_mouse_pos;
		bool m_freecam_enabled = false;

		float m_speed = 4.0f;
		
	};

}

