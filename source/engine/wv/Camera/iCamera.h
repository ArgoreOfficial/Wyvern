#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

struct sInputInfo;

namespace wv
{
	
	class iCamera
	{
	public:
		enum eCameraType
		{
			CameraType_Perspective,
			CameraType_Orthographic
		};

		iCamera( eCameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 100.0f );

		~iCamera( void );

		virtual void onRawInput( sInputInfo* _info ) { }
		virtual void update( double _delta_time ) { }

		glm::mat4 getProjectionMatrix( void );

		wv::cTransformf& getTransform( void ) { return m_transform; }
		glm::mat4 getViewMatrix( void );
	
	protected:
		glm::mat4 getPerspectiveMatrix ( void );
		glm::mat4 getOrthographicMatrix( void );

		float m_fov  = 60.0f;
		float m_near = 0.01f;
		float m_far  = 100.0f;

		wv::cTransformf m_transform;
		eCameraType m_type = CameraType_Perspective;
	};

}

