#pragma once
#include <Wyvern/API/Core.h>
#include "GLHeader.h"
#include "RenderObject.h"

class WYVERN_API Renderer
{
public:
	void Clear() const;
	void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shaderProgram);
	void Draw( const RenderObject& renderObject );
};