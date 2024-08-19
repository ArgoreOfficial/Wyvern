#pragma once
#include <wv/Math/Vector3.h>

/// TODO: change to wv/Math/Matrix
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv 
{

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
    class Transform
    {

	public:

		inline void setPosition( wv::Vector3<T> _position ) { position = _position; }
		inline void setRotation( wv::Vector3<T> _rotation ) { rotation = _rotation; }
		inline void setScale   ( wv::Vector3<T> _scale )    { scale = _scale; }
		
		inline void translate( wv::Vector3<T> _translation ) { position += _translation; }
		inline void rotate   ( wv::Vector3<T> _rotation )    { rotation += _rotation; }
		
		inline glm::mat<4, 4, T> getMatrix() { return m_matrix; }

		void update()
		{
			glm::mat<4, 4, T> model( 1 );

			model = glm::translate( model, glm::vec<3, T>{ position.x, position.y, position.z } );

			model = glm::rotate<T>( model, glm::radians( rotation.y ), glm::vec<3, T>{ 0, 1, 0 } ); // yaw
			model = glm::rotate<T>( model, glm::radians( rotation.x ), glm::vec<3, T>{ 1, 0, 0 } ); // pitch
			model = glm::rotate<T>( model, glm::radians( rotation.z ), glm::vec<3, T>{ 0, 0, 1 } ); // roll

			model = glm::scale( model, glm::vec<3, T>{ scale.x, scale.y, scale.z } );

			if( parent != nullptr )
				model = parent->getMatrix() * model;

			m_matrix = model;
		}

		inline Vector3<T> forward()
		{
			return rotation.eulerToDirection();
		}

///////////////////////////////////////////////////////////////////////////////////////

		Transform<T>* parent = nullptr;
		Vector3<T> position{ 0, 0, 0 };
		Vector3<T> rotation{ 0, 0, 0 };
		Vector3<T> scale   { 1, 1, 1 };

	private:

		glm::mat<4, 4, T> m_matrix{ 1 };
    };

///////////////////////////////////////////////////////////////////////////////////////

	typedef Transform<float>  Transformf;
	typedef Transform<double> Transformd;
	typedef Transform<int>    Transformi;

}