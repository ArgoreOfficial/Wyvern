#include <stdio.h>

#include "Window/cWindow.h"
#include "Renderer/cRenderer.h"

#include "Renderer/Framework/cVertexBuffer.h"
#include "Renderer/Primitive/InputLayout.h"

int main()
{
	cWindow window;
	cRenderer renderer;

	window.create( 800, 600, "Triangle" );
	renderer.create( window );
	
	float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
	};

	//lo::cBuffer<lo::BufferType_Vertex, lo::Float3, lo::Float3, lo::Float2> vertex_buffer;
	//vertex_buffer.buffer( vertices, 3 );

	// vertex buffer
		
		// vertex array object
		unsigned int vao;
		glGenVertexArrays( 1, &vao );
		glBindVertexArray( vao );


		// create vertex buffer object
		unsigned int vbo;
		glGenBuffers( 1, &vbo );
		glBindBuffer( GL_ARRAY_BUFFER, vbo );
		// bind data
		glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
		
		// set layout element 0
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void*)0 );
		// enable layout element 0
		glEnableVertexAttribArray( 0 );

		
		

	while ( !window.shouldClose() )
	{
		window.startFrame();
		renderer.clear( 0x000000FF );

		glUseProgram( renderer.getDefaultShader() );
		glBindVertexArray( vao );
		glDrawArrays( GL_TRIANGLES, 0, 3 );

		window.endFrame();
	}

	glfwTerminate();
}