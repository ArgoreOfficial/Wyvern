#include <stdio.h>

#include "Window/cWindow.h"
#include <lo/cRenderer.h>

#include <lo/Framework/cVertexBuffer.h>
#include <lo/Primitive/InputLayout.h>
#include <lo/Primitive/Buffer.h>
#include <lo/RenderState/PipelineState.h>
#include <lo/sCmd.h>

int main()
{
	cWindow window;
	lo::cRenderer renderer;

	window.create( 800, 600, "Triangle" );
	renderer.create( window );
	



	// vertex buffer
	float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
	};

	lo::sInputLayoutElement layout[] = {
		lo::sInputLayoutElement{ 3, lo::LO_TYPE_FLOAT, false, 0 },               // position
		lo::sInputLayoutElement{ 3, lo::LO_TYPE_FLOAT, false, 0 },               // colour
		lo::sInputLayoutElement{ 2, lo::LO_TYPE_FLOAT, true, 0 },                // texcoord
	};

	lo::sInputLayoutInfo layoutinfo{
		.elements = layout,
		.num_elements = 3
	};

	//float vertices[] = {
	//		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
	//		 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	//		 0.0f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f
	//};
	
	lo::sBufferDesc vb_desc {
		.type   = lo::LO_BUFFER_TYPE_VERTEX,
		.usage  = lo::LO_BUFFER_USAGE_STATIC_DRAW,
		.layout = &layoutinfo,
		.data   = vertices,
		.size   = sizeof( vertices )
	};

	lo::sPipelineStateDesc ps_desc;
	ps_desc.drawMode = lo::LO_PIPELINE_DRAW_MODE_ARRAY;
	ps_desc.type     = lo::LO_PRIMITIVE_TRIANGLES;
	ps_desc.cullFace = lo::LO_CULL_NONE;
	ps_desc.vertexBufferDesc = &vb_desc;
	ps_desc.indexBufferDesc  = nullptr;
	ps_desc.program  = renderer.getDefaultShader();
	ps_desc.hasAlpha = false;

	lo::cPipelineState triangle = renderer.createPipelineState( &ps_desc );

	// vertex array object
	unsigned int vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	// create vertex buffer object
	unsigned int vbo;
	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );

	// buffer data
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void*)0 );
	glEnableVertexAttribArray( 0 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	// vertex buffer
	
	lo::sCmd commands[]
	{
		// clear
		lo::sCmd{ lo::LO_CMD_TYPE_CLEAR, 0x000000FF },
		// bind 
		lo::sCmd{ lo::LO_CMD_TYPE_BIND_SHADER, renderer.getDefaultShader() },
		lo::sCmd{ lo::LO_CMD_TYPE_BIND_VERTEX_ARRAY_OBJECT, vao },
		// draw
		lo::sCmd{ lo::LO_CMD_TYPE_BIND_DRAW }
	};

	while ( !window.shouldClose() )
	{
		window.startFrame();

		for ( int i = 0; i < 4; i++ )
			renderer.execCmd( commands[ i ] );
		
		window.endFrame();
	}

	glfwTerminate();
}