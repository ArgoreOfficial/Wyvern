#pragma once

#include <wv/Graphics/cMesh.h>
#include <cm/Framework/Texture.h>
#include <cm/Framework/Shader.h>

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

		void render();

	private:

		wv::cMesh*         m_quad;
		
		/* change to material */
		cm::sTexture2D     m_texture; 
		cm::hShaderProgram m_shader_program;

	};
}