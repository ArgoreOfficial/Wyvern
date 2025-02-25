#pragma once

#include <wv/math/transform.h>
#include <wv/math/vector3.h>
#include <wv/math/vector2.h>

#include <wv/decl.h>
#include <wv/graphics/types.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	class ICamera
	{

	WV_DECLARE_INTERFACE( ICamera )

	public:

		enum CameraType
		{
			WV_CAMERA_TYPE_PERSPECTIVE,
			WV_CAMERA_TYPE_ORTHOGRAPHIC
		};

		ICamera( CameraType _type, float _fov = 60.0f, float _near = 0.01f, float _far = 10000.0f );

		virtual void onEnter() { }
		virtual void update( double /*_delta_time*/ ) { m_transform.update( nullptr ); }

		bool beginRender( IGraphicsDevice* _pGraphicsDevice, FillMode _fillMode );

		Matrix4x4f getProjectionMatrix( void );

		Matrix4x4f getViewMatrix( void );

		Transformf& getTransform( void ) { return m_transform; }
		Vector3f getViewDirection();

		GPUBufferID getBufferID() { return m_uniformBufferID; }

///////////////////////////////////////////////////////////////////////////////////////

		float fov  = 60.0f;

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		Matrix4x4f getPerspectiveMatrix( void );
		Matrix4x4f getOrthographicMatrix( void );

///////////////////////////////////////////////////////////////////////////////////////

		GPUBufferID m_uniformBufferID;

		float m_near = 0.01f;
		float m_far  = 100.0f;

		wv::Transformf m_transform;
		CameraType m_type = WV_CAMERA_TYPE_PERSPECTIVE;
	};

}

