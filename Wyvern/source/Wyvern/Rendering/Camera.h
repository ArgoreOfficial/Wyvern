#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace WV
{
	class Camera
	{
	private:
		glm::vec3 position;
		glm::vec3 rotation;
		float fov;
		float aspect;
		glm::mat4 projectionMatrix;

	public:
		Camera();
		Camera( float _fov, int _width, int _height );
		~Camera();


		glm::mat4 getViewMatrix();
		void setProjectionMatrix( float _fov, float _aspect, float _near = 0.1f, float _far = 100.0f );
		void move( glm::vec3 _direction );
		void rotate( glm::vec3 _angle );
		glm::mat4 getProjectionMatrix() { return projectionMatrix; }

	};

}