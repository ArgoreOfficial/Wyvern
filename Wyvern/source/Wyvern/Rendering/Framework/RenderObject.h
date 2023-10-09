#pragma once
#include "GLHeader.h"
#include <string>
#include <sstream>
#include <fstream>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Material.h"

namespace WV
{
	class Camera;

	struct Mesh
	{
		VertexArray* vertexArray;
		IndexBuffer* indexBuffer;
	};

	class RenderObject
	{
	private:
		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;

		Mesh* m_mesh;
		Material* m_material;
	public:
		RenderObject( Mesh* _mesh, Material* _material );
		~RenderObject();

		void bind( Camera& _camera );

		VertexArray& getVertexArray() { return *(m_mesh->vertexArray); }
		IndexBuffer& getIndexBuffer() { return *(m_mesh->indexBuffer); }
		Material& getMaterial() { return *m_material; }

		static RenderObject* createFromFiles( const char* _meshPath, const char* _shaderPath, const char* _texturePath );

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