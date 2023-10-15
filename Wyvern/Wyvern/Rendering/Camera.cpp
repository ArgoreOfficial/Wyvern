#include "Camera.h"
using namespace WV;

Camera::Camera() :
	position( 0, 0, 0 ),
	rotation( 0, 0, 0 ),
	fov(0.0f),
	aspect(16.0f / 9.0f)
{ }

Camera::Camera( float _fov, int _width, int _height ) :
	position( 0, 0, 0 ),
	rotation( 0, 0, 0 )
{
	fov = _fov;
	aspect = (float(_width) / float(_height));
	setProjectionMatrix( _fov, aspect );
}

Camera::~Camera()
{ }


glm::mat4 Camera::getViewMatrix()
{
	glm::mat4 view = glm::mat4( 1.0f );
	view = glm::rotate( view, glm::radians( rotation.x ), glm::vec3( 1.0, 0.0, 0.0 ) );
	view = glm::rotate( view, glm::radians( rotation.y ), glm::vec3( 0.0, 1.0, 0.0 ) );
	view = glm::rotate( view, glm::radians( rotation.z ), glm::vec3( 0.0, 0.0, 1.0 ) );
	view = glm::translate( view, -position );
	return view;
}

void Camera::setProjectionMatrix( float _fov, float _aspect, float _near, float _far)
{ 
	projectionMatrix = glm::perspective<float>( glm::radians( _fov ), _aspect, _near, _far );
}

void Camera::move( glm::vec3 _direction )
{
	glm::mat4 mat = glm::mat4( 1.0f );
	mat = glm::rotate( mat, glm::radians( rotation.x ), glm::vec3( 1.0, 0.0, 0.0 ) );
	mat = glm::rotate( mat, glm::radians( rotation.y ), glm::vec3( 0.0, 1.0, 0.0 ) );
	mat = glm::rotate( mat, glm::radians( rotation.z ), glm::vec3( 0.0, 0.0, 1.0 ) );

	position += glm::vec3( glm::vec4( _direction, 0 ) * mat );
}

void Camera::rotate( glm::vec3 _angle )
{
	rotation += _angle;
	if ( rotation.x >= 89 ) rotation.x = 89;
	if ( rotation.x <= -89 ) rotation.x = -89;
}
