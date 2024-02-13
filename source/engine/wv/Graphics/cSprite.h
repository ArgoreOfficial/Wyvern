#pragma once

#include <cm/Framework/Shader.h>
#include <cm/Framework/Texture.h>

#include <wv/Graphics/cMesh.h>
#include <wv/Math/Transform.h>
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

		wv::cTransformf& getTransform( void ) { return m_transform; }

		void render( void );

	private:

		wv::cMesh*         m_quad;
		
		/* change to material */
		cm::sTexture2D     m_texture; 
		cm::hShaderProgram m_shader_program;

		wv::cTransformf m_transform;

	};
}