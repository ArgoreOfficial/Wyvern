#pragma once

class cWindow;

#include <lo/RenderState/PipelineState.h>
#include <lo/sCmd.h>

namespace lo
{
	
	class cRenderer
	{
	public:
		cRenderer();
		~cRenderer();
	
		bool create( cWindow& _window );
		void clear( unsigned char _color );
		unsigned int getDefaultShader( void ) const { return m_shader_default; }
		cPipelineState createPipelineState( sPipelineStateDesc* _desc );
		void execCmd( sCmd _cmd );

	private:

		void createDefaultShader( void );

		const char* m_vertex_shader_default_source = 
			"#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";

		const char* m_fragment_shader_default_source =
			"#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			"}\0";

		unsigned int m_shader_default = 0;
	};
}

