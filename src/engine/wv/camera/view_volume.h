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

		inline void recalculateViewMatrix( Transformf* _parent, bool _cacheViewProj = true ) {
			m_transform.update( _parent );
			m_viewMatrix = m_transform.getMatrix().inverse();

			if ( _cacheViewProj )
				m_viewProjMatrix = m_viewMatrix * m_projMatrix;
		}

		inline void recalculateProjMatrix( bool _cacheViewProj = true ) {
			switch ( m_type )
			{
			case kPerspective:  m_projMatrix = calculatePerspectiveMatrix();  break;
			case kOrthographic: m_projMatrix = calculateOrthographicMatrix(); break;
			}

			if ( _cacheViewProj )
				m_viewProjMatrix = m_viewMatrix * m_projMatrix;
		}

		inline void setViewDimensions( const wv::Vector2f& _vec ) { 
			m_viewDimensions = _vec; 
			m_aspect = _vec.x / _vec.y;
		}

		inline void setViewDimensions( size_t _width, size_t _height ) { 
			setViewDimensions( { (float)_width, (float)_height } );
		}

		Vector3f screenToWorld( int _pixelX,  int _pixelY,  float _depth );
		Vector3f screenToWorld( float _clipX, float _clipY, float _depth );

		Matrix4x4f  getViewMatrix()       const { return m_viewMatrix; }
		Matrix4x4f  getProjectionMatrix() const { return m_projMatrix; }
		Matrix4x4f  getViewProjMatrix()   const { return m_viewProjMatrix; }
		Vector3f    getViewDirection()    const;

		wv::Vector2f getViewDimensions() const { return m_viewDimensions; }
		size_t getPixelWidth() const { return m_viewDimensions.x; }
		size_t getPixelHeight() const { return m_viewDimensions.y; }

		Transformf& getTransform( void ) { return m_transform; }
		
		void  setOrthoScale( float _scale ) { m_orthoScale = _scale; }
		float getOrthoScale() const { return m_orthoScale; }

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		Matrix4x4f calculatePerspectiveMatrix()  const;
		Matrix4x4f calculateOrthographicMatrix() const;

///////////////////////////////////////////////////////////////////////////////////////

		float fov = 60.0f;
		
		float m_near = 0.01f;
		float m_far  = 100.0f;

		float m_orthoScale = 0.1f;

		wv::Vector2f m_viewDimensions{ 900.0f, 600.0f };
		float m_aspect = 1.777777778f;

		wv::Transformf m_transform;
		CameraType m_type = kPerspective;

		Matrix4x4f m_viewMatrix{};
		Matrix4x4f m_projMatrix{};
		Matrix4x4f m_viewProjMatrix{};
	};

}

