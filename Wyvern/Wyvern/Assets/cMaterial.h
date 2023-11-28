#pragma once

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
		void load();
		void use();

		cShaderProgram& getShader() { return m_shader; }

	private:

		cTexture2D* m_texture = nullptr;
		cShaderProgram m_shader;

	};
}