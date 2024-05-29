#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

#include <wv/Decl.h>

struct sInputInfo;

namespace wv
{
	
	class ICamera
	{
	WV_DECLARE_INTERFACE( ICamera )

	public:
		enum CameraType
		{
			WV_CAMERA_TYPE_PERSPECTIVE,
			WV_CAMERA_TYPE_ORTHOGRAPHIC
		};

		ICamera( CameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 100.0f );

		virtual void onCreate() { }
		virtual void update( double _delta_time ) { }

		glm::mat4 getProjectionMatrix( void );
		glm::mat4 getViewMatrix( void );

		Transformf& getTransform( void ) { return m_transform; }
		Vector3f getViewDirection();

		float fov  = 60.0f;

	protected:
		glm::mat4 getPerspectiveMatrix ( void );
		glm::mat4 getOrthographicMatrix( void );

		float m_near = 0.01f;
		float m_far  = 100.0f;

		wv::Transformf m_transform;
		CameraType m_type = WV_CAMERA_TYPE_PERSPECTIVE;
	};

}

