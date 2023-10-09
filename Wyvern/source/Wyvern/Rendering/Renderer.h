#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <Wyvern/Rendering/Camera.h>
#include "Framework/GLHeader.h"
#include "Framework/RenderObject.h"

namespace WV
{
	class Renderer : public ISingleton<Renderer>
	{
	public:
		void Clear() const;
		
		static void draw( const VertexArray& _vertexArray, const IndexBuffer& _indexBuffer, const ShaderProgram& _shaderProgram )
		{
			getInstance().internalDraw( _vertexArray, _indexBuffer, _shaderProgram );
		}

		static void draw( RenderObject& _renderObject ) { getInstance().internalDraw( _renderObject ); }
		static void setActiveCamera( Camera* _camera ) { getInstance().m_activeCamera = _camera; }

	private:
		void internalDraw( const VertexArray& _vertexArray, const IndexBuffer& _indexBuffer, const ShaderProgram& _shaderProgram );
		void internalDraw( RenderObject& _renderObject );

		Camera* m_activeCamera = nullptr;
	};
}