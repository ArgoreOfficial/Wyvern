#pragma once

#include <Wyvern/Assets/cResourceHandle.h>
#include <Wyvern/Renderer/Framework/cTexture2D.h>
#include <Wyvern/Renderer/Framework/cShaderProgram.h>

namespace wv
{

	class cMaterial
	{
	public:
		
		 cMaterial() { }
		~cMaterial() { }
	
		void loadTexture( std::string _path );
		
		void load  ( void );
		void create( void );
		void use   ( void );

		cShaderProgram& getShader() { return m_shader; }

	private:

		cShaderSource m_vertSource;
		cShaderSource m_fragSource;

		bool m_hasTexture;
		std::string m_texturePath = "";

		tTextureHandle m_textureHandle;
		cTexture2D* m_texture = nullptr;
		cShaderProgram m_shader;

	};
}