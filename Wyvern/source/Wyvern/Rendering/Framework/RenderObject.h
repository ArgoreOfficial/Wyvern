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
	class RenderObject
	{
	private:
		glm::vec3 _position;
		glm::vec3 _rotation;
		glm::vec3 _scale;

		VertexArray* _vertexArray;
		IndexBuffer* _indexBuffer;

		ShaderProgram* _shaderProgram; // change to material later
		Texture* _texture;

	public:
		RenderObject( VertexArray* vertexArray, IndexBuffer* indexBuffer, ShaderProgram* shaderProgram, Texture* texture );
		~RenderObject();

		const VertexArray& GetVertexArray() const { return *_vertexArray; }
		const IndexBuffer& GetIndexBuffer() const { return *_indexBuffer; }
		const ShaderProgram& GetShaderProgram() const { return *_shaderProgram; }
		const Texture& GetTexture() const { return *_texture; }

		static RenderObject* CreateFromFiles( std::string meshPath, std::string shaderPath, std::string texturePath );
	};
}