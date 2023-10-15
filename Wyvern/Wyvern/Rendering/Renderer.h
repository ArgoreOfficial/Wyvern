#pragma once
#include "Framework/GLHeader.h"
#include <Wyvern/Core/ISingleton.h>
#include <Wyvern/Rendering/Camera.h>
#include "Framework/Model.h"

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

		static void draw( Model& _renderObject ) { getInstance().internalDraw( _renderObject ); }
		static void setActiveCamera( Camera* _camera ) { getInstance().m_activeCamera = _camera; }

	private:
		void internalDraw( const VertexArray& _vertexArray, const IndexBuffer& _indexBuffer, const ShaderProgram& _shaderProgram );
		void internalDraw( Model& _renderObject );

		Camera* m_activeCamera = nullptr;
	};
}