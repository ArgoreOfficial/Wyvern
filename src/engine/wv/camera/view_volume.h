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

	class ViewVolume
	{
	public:

		enum CameraType
		{
			kPerspective,
			kOrthographic
		};

		ViewVolume( CameraType _type, size_t _width, size_t _height, float _fov = 60.0f, float _near = 0.01f, float _far = 10000.0f );

		virtual void update( double /*_delta_time*/ ) { m_transform.update( nullptr ); }

		inline void setViewDimensions( const wv::Vector2f& _vec ) { 
			m_viewDimensions = _vec; 
			m_aspect = _vec.x / _vec.y;
		}

		inline void setViewDimensions( size_t _width, size_t _height ) { 
			setViewDimensions( { (float)_width, (float)_height } );
		}

		Vector3f screenToWorld( int _pixelX,  int _pixelY,  float _depth );
		Vector3f screenToWorld( float _clipX, float _clipY, float _depth );

		Matrix4x4f  getProjectionMatrix() const;
		Matrix4x4f  getViewMatrix()       const;
		Vector3f    getViewDirection()    const;

		inline Matrix4x4f getViewProjMatrix() const {
			return getViewMatrix() * getProjectionMatrix();
		}

		float getOrthoWidth () { return m_ortho_width; }
		float getOrthoHeight() { return m_ortho_height; }

		wv::Vector2f getViewDimensions() const { return m_viewDimensions; }
		size_t getPixelWidth() const { return m_viewDimensions.x; }
		size_t getPixelHeight() const { return m_viewDimensions.y; }

		Transformf& getTransform( void ) { return m_transform; }
		
		// will override height
		void setOrthoWidth( float _width ); 

		// will override width
		void setOrthoHeight( float _height );

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		Matrix4x4f getPerspectiveMatrix()      const;
		Matrix4x4f getOrthographicMatrix()     const;

///////////////////////////////////////////////////////////////////////////////////////

		float fov = 60.0f;
		
		float m_near = 0.01f;
		float m_far  = 100.0f;

		float m_ortho_width  = 1.0f;
		float m_ortho_height = 1.0f;

		wv::Vector2f m_viewDimensions{ 900.0f, 600.0f };
		float m_aspect = 1.777777778f;

		wv::Transformf m_transform;
		CameraType m_type = kPerspective;
	};

}

