#include "Renderer.h"
#include <Wyvern/Logging/Logging.h>
using namespace WV;

void Renderer::Clear() const
{
	glClearColor( 0.392f, 0.584f, 0.929f, 1.0f ); // cornflower blue
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Renderer::internalDraw( const VertexArray& _vertexArray, const IndexBuffer& _indexBuffer, const ShaderProgram& _shaderProgram )
{
	_shaderProgram.Bind();
	_vertexArray.Bind();
	_indexBuffer.Bind();

	glDrawElements( GL_TRIANGLES, _indexBuffer.GetIndexCount(), GL_UNSIGNED_INT, nullptr );
}

void Renderer::internalDraw( RenderObject& _renderObject )
{
	if ( !m_activeCamera )
	{
		WVFATAL( "No Active Camera!" );
		return;
	}
	_renderObject.bind( *m_activeCamera);

	glDrawElements( GL_TRIANGLES, _renderObject.getIndexBuffer().GetIndexCount(), GL_UNSIGNED_INT, nullptr );
}
