#include "Renderer.h"
using namespace WV;

void Renderer::Clear() const
{ 
	glClearColor( 0.392f, 0.584f, 0.929f, 1.0f ); // cornflower blue
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Renderer::Draw( const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shaderProgram )
{ 
	shaderProgram.Bind();
	vertexArray.Bind();
	indexBuffer.Bind();

	glDrawElements( GL_TRIANGLES, indexBuffer.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Draw( const RenderObject& renderObject )
{
	renderObject.GetShaderProgram().Bind();
	renderObject.GetVertexArray().Bind();
	renderObject.GetIndexBuffer().Bind();

	glDrawElements( GL_TRIANGLES, renderObject.GetIndexBuffer().GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}
