#pragma once
#include "GLHeader.h"
#include "RenderObject.h"

class Renderer
{
public:
	void Clear() const;
	void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shaderProgram);
	void Draw( const RenderObject& renderObject );
};