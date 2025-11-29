#pragma once

#include <wv/math/transform.h>
#include <wv/math/vector3.h>
#include <wv/math/vector2.h>

#include <wv/decl.h>

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
			kPerspective,
			kFocal,
			kOrthographic
		};

		ICamera( CameraType _type, size_t _width, size_t _height, float _fov = 60.0f, float _near = 0.01f, float _far = 10000.0f );

		virtual void onEnter() { }
		virtual void update( double /*_delta_time*/ ) { m_transform.update( nullptr ); }

		void setPixelSize( size_t _width, size_t _height );
		void setPixelSize( size_t _width, float _aspect );

		Vector3f screenToWorld( int _pixelX,  int _pixelY,  float _depth );
		Vector3f screenToWorld( float _clipX, float _clipY, float _depth );

		Matrix4x4f  getProjectionMatrix( void );
		Matrix4x4f  getViewMatrix      ( void );
		Vector3f    getViewDirection   ( void );
		
		float getOrthoWidth ( void ) { return m_ortho_width; }
		float getOrthoHeight( void ) { return m_ortho_height; }

		size_t getPixelWidth ( void ) { return m_pixelWidth; }
		size_t getPixelHeight( void ) { return m_pixelHeight; }

		Transformf& getTransform( void ) { return m_transform; }
		
		// will override height
		void setOrthoWidth( float _width ); 

		// will override width
		void setOrthoHeight( float _height );

		void setFocalSize( float _sensorWidth, float _sensorHeight, float _focalLength );

///////////////////////////////////////////////////////////////////////////////////////

		float fov = 60.0f;
		
		// only used for focal perspective
		
///////////////////////////////////////////////////////////////////////////////////////

	protected:

		Matrix4x4f getPerspectiveMatrix     ( void );
		Matrix4x4f getFocalPerspectiveMatrix( void );
		Matrix4x4f getOrthographicMatrix    ( void );

///////////////////////////////////////////////////////////////////////////////////////

		float m_focalLength = 30.0f;
		float m_focalSensorWidth  = 36.0f;
		float m_focalSensorHeight = 24.0f;

		float m_near = 0.01f;
		float m_far  = 100.0f;

		float m_ortho_width  = 1.0f;
		float m_ortho_height = 1.0f;

		float m_pixelWidth  = 1920.0f;
		float m_pixelHeight = 1080.0f;
		float m_aspect = 1.777777778f;

		wv::Transformf m_transform;
		CameraType m_type = kPerspective;
	};

}

