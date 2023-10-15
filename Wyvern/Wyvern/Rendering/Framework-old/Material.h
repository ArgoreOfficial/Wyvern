#pragma once
#include <Wyvern/Rendering/Framework/ShaderProgram.h>
#include <Wyvern/Rendering/Framework/Texture.h>
#include <glm/glm.hpp>

namespace WV
{
	struct Diffuse
	{
		glm::vec4 color;
		Texture* texture;
	};

	class Material
	{
	public:
		Material( ShaderProgram* _shaderProgram, Diffuse _diffuse, glm::vec4 _specular ) :
			m_shaderProgram(_shaderProgram), 
			m_diffuse( _diffuse ), 
			m_specular( _specular ), 
			m_UVOffset( 0.0f, 0.0f ),
			m_tileCount( 1.0f, 1.0f ) { }
		
		~Material() { }

		Diffuse getDiffuse() { return m_diffuse; }
		glm::vec4 getSpecular() { return m_specular; }
		inline void setUVOffset( glm::vec2 _offset ) { m_UVOffset = _offset; }
		inline void setTileCount( glm::vec2 _offset ) { m_tileCount = _offset; }

		inline void bind(glm::mat4 _projection, glm::mat4 _view, glm::mat4 _model)
		{
			m_shaderProgram->Bind();

			if ( m_diffuse.texture )
			{
				m_diffuse.texture->Bind();
				m_shaderProgram->SetUniform1i( "u_hasTexture", 1 );
			}
			else
			{
				m_shaderProgram->SetUniform1i( "u_hasTexture", 0 );
			}

			m_shaderProgram->SetUniform4f( "u_Color", m_diffuse.color );
			m_shaderProgram->SetUniformMat4( "u_Projection", _projection );
			m_shaderProgram->SetUniformMat4( "u_View", _view );
			m_shaderProgram->SetUniformMat4( "u_Model", _model );
			m_shaderProgram->SetUniform2f( "u_UVOffset", m_UVOffset );
			m_shaderProgram->SetUniform2f( "u_tileCount", m_tileCount );
		}

	private:
		ShaderProgram* m_shaderProgram;
		Diffuse m_diffuse;
		glm::vec4 m_specular;
		glm::vec2 m_UVOffset;
		glm::vec2 m_tileCount;
	};
}
