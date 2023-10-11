#pragma once
#include "GLHeader.h"
#include <string>
#include <sstream>
#include <fstream>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "Mesh.h"

namespace WV
{
	class Camera;

	class Model
	{
	public:
		Model( Mesh* _mesh, Material* _material );
		~Model();

		void bind( Camera& _camera );

		VertexArray& getVertexArray() { return *( m_mesh->getVertexArray() ); }
		IndexBuffer& getIndexBuffer() { return *( m_mesh->getIndexBuffer() ); }
		Material& getMaterial() { return *m_material; }

		static Model* createFromFiles( const char* _meshPath, const char* _shaderPath, const char* _texturePath );

		glm::vec3 getPosition() { return m_position; }
		glm::vec3 getRotation() { return m_rotation; }
		glm::vec3 getScale() { return m_scale; }

		void setPosition( glm::vec3 _position ) { m_position = _position; }
		void setRotation( glm::vec3 _rotation ) { m_rotation = _rotation; }
		void setScale( glm::vec3 _scale ) { m_scale = _scale; }
		void move( glm::vec3 _move ) { m_position += _move; }
		void rotate( glm::vec3 _rotate ) { m_rotation += _rotate; }
		void scale( glm::vec3 _scale ) { m_scale *= _scale; }

	private:
		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;

		Mesh* m_mesh;
		Material* m_material;
	};
}