#pragma once
#include "GLHeader.h"
#include <string>
#include <sstream>
#include <fstream>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ShaderProgram.h"
#include "Texture.h"

namespace WV
{
	class Camera;

	class RenderObject
	{
	private:
		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;

		VertexArray* m_vertexArray;
		IndexBuffer* m_indexBuffer;

		ShaderProgram* m_shaderProgram; // change to material later
		Texture* m_texture;

	public:
		RenderObject( VertexArray* _vertexArray, IndexBuffer* _indexBuffer, ShaderProgram* _shaderProgram, Texture* _texture );
		~RenderObject();

		void bind( Camera& _camera );

		VertexArray& getVertexArray() { return *m_vertexArray; }
		IndexBuffer& getIndexBuffer() { return *m_indexBuffer; }
		ShaderProgram& getShaderProgram() { return *m_shaderProgram; }
		Texture& getTexture() { return *m_texture; }

		static RenderObject* createFromFiles( std::string meshPath, std::string shaderPath, std::string texturePath );

		glm::vec3 getPosition() { return m_position; }
		glm::vec3 getRotation() { return m_rotation; }
		glm::vec3 getScale() { return m_scale; }

		void setPosition( glm::vec3 _position ) { m_position = _position; }
		void setRotation( glm::vec3 _rotation ) { m_rotation = _rotation; }
		void setScale( glm::vec3 _scale ) { m_scale = _scale; }
		void move( glm::vec3 _move ) { m_position += _move; }
		void rotate( glm::vec3 _rotate ) { m_rotation += _rotate; }
		void scale( glm::vec3 _scale ) { m_scale *= _scale; }
	};
}