#pragma once

#include <wv/Core/iSingleton.h>

#include <cm/Framework/Shader.h>
#include <cm/Framework/Buffer.h>

namespace cm { class cWindow; }
namespace cm { class iBackend; }
namespace cm { class iShader; }

namespace wv
{
	
	class cRenderer : public iSingleton<cRenderer>
	{
	public:
		enum eBackendType
		{
			BackendType_OpenGL,
			BackendType_Direct3D11
		};

		 cRenderer( void );
		~cRenderer( void );

		void create() override;
		
		void onResize( int _width, int _height );
		void clear   ( unsigned int _color );

		void begin( void );
		void end  ( void );

		void setBackendType( cRenderer::eBackendType _backend ) { m_backend_type = _backend; }

		cm::Shader::hShaderProgram getDefaultShader( void ) const { return m_shader_default; }
		cm::iBackend* getBackend           ( void ) { return m_backend; }

	private:

		void createDefaultShader( void );

		eBackendType m_backend_type = eBackendType::BackendType_OpenGL;
		cm::iBackend* m_backend     = nullptr;

		const char* m_shader_default_vert =
			"#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"uniform mat4 proj;\n"
			"uniform mat4 view;\n"
			"uniform mat4 model;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = proj * view * model * vec4(aPos, 1.0);\n"
			"}\0";

		const char* m_shader_default_frag =
			"#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"	FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );\n"
			"}\0";

		cm::Shader::hShaderProgram m_shader_default;
	};

}
