#pragma once
#include <Wyvern/Rendering/Mesh.h>

namespace WV
{
	class Model
	{
	public:
		Model( Mesh& _mesh );
		~Model();

		void setPosition(float _x, float _y, float _z);
		void setRotation( float _x, float _y, float _z );
		void setScale( float _x, float _y, float _z );

		void move( float _x, float _y, float _z );
		void rotate( float _x, float _y, float _z );

		void submit( float _aspect );

	private:
		float m_rotationX = 0.0f;
		float m_rotationY = 0.0f;
		float m_rotationZ = 0.0f;

		float m_positionX = 0.0f;
		float m_positionY = 0.0f;
		float m_positionZ = 0.0f;

		float m_scaleX = 1.0f;
		float m_scaleY = 1.0f;
		float m_scaleZ = 1.0f;
		/*
		bgfx::VertexLayout m_layout;
		bgfx::VertexBufferHandle m_vertexBufferHandle;
		bgfx::IndexBufferHandle m_indexBufferHandle;
		bgfx::ShaderHandle m_vertexShaderHandle;
		bgfx::ShaderHandle m_fragmentShaderHandle;
		bgfx::ProgramHandle m_program;
		*/
	};
}