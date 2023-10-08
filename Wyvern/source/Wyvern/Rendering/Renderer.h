#pragma once
#include <Wyvern/Core/ISingleton.h>
#include "Framework/GLHeader.h"
#include "Framework/RenderObject.h"

namespace WV
{
	class Renderer : ISingleton<Renderer>
	{
	public:
		void Clear() const;
		void Draw( const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shaderProgram );
		void Draw( const RenderObject& renderObject );
	};
}