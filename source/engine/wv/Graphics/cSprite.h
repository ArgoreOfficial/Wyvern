#pragma once

#include <cm/Framework/Shader.h>
#include <cm/Framework/Texture.h>

#include <wv/Graphics/cMesh.h>
#include <wv/Math/Vector3.h>

#include <string>

namespace wv
{
	class cSprite
	{
	public:

		 cSprite( void );
		~cSprite( void );

		/* abstract further? */
		void create( const std::string& _texture_path );


		/* move to cTransform */
		void setPosition     ( wv::cVector3f _position ) { m_position = _position; }
		void setScale        ( wv::cVector3f _scale )    { m_scale    = _scale; }
		void setRotation     ( float _rotation )         { m_rotation = _rotation; }
		void setRotationEuler( float _rotation )         { m_rotation = _rotation * ( 3.1415f / 180.0f ); }

		void translate  ( wv::cVector3f _translation ) { m_position += _translation; }
		void scale      ( float _scalar )           { m_scale *= _scalar; }
		void rotate     ( float _rotation )         { m_rotation += _rotation; }
		void rotateEuler( float _rotation )         { m_rotation += _rotation * ( 3.1415f / 180.0f ); } // move PI to Math/Constants

		void render();

	private:

		wv::cMesh*         m_quad;
		
		/* change to material */
		cm::sTexture2D     m_texture; 
		cm::hShaderProgram m_shader_program;

		wv::cVector3f m_position;
		wv::cVector3f m_scale;
		float         m_rotation;

	};
}