#pragma once

#include <wv/Math/Transform.h>
#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

#include <wv/Decl.h>
#include <wv/Graphics/Types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iLowLevelGraphics;

///////////////////////////////////////////////////////////////////////////////////////

	class iCamera
	{

	WV_DECLARE_INTERFACE( iCamera )

	public:

		enum CameraType
		{
			WV_CAMERA_TYPE_PERSPECTIVE,
			WV_CAMERA_TYPE_ORTHOGRAPHIC
		};

		iCamera( CameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 10000.0f );

		virtual void onCreate() { }
		virtual void update( double _delta_time ) { m_transform.update( nullptr ); }

		bool beginRender( iLowLevelGraphics* _pLowLevelGraphics, eFillMode _fillMode );

		cMatrix4x4f getProjectionMatrix( void );

		cMatrix4x4f getViewMatrix( void );

		Transformf& getTransform( void ) { return m_transform; }
		cVector3f getViewDirection();

		GPUBufferID getBufferID() { return m_uniformBufferID; }

///////////////////////////////////////////////////////////////////////////////////////

		float fov  = 60.0f;

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		cMatrix4x4f getPerspectiveMatrix( void );
		cMatrix4x4f getOrthographicMatrix( void );

///////////////////////////////////////////////////////////////////////////////////////

		GPUBufferID m_uniformBufferID;

		float m_near = 0.01f;
		float m_far  = 100.0f;

		wv::Transformf m_transform;
		CameraType m_type = WV_CAMERA_TYPE_PERSPECTIVE;
	};

}

