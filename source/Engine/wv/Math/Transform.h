#pragma once
#include <wv/Math/Vector3.h>

/// TODO: change to wv/Math/Matrix
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <wv/Math/Matrix.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv 
{

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
    class Transform
    {

	public:

		inline void setPosition( wv::cVector3<T> _position ) { position = _position; }
		inline void setRotation( wv::cVector3<T> _rotation ) { rotation = _rotation; }
		inline void setScale   ( wv::cVector3<T> _scale )    { scale = _scale; }
		
		inline void translate( wv::cVector3<T> _translation ) { position += _translation; }
		inline void rotate   ( wv::cVector3<T> _rotation )    { rotation += _rotation; }
		
		inline glm::mat<4, 4, T> getMatrix() { return m_matrix; }
		inline cMatrix<T, 4, 4> getWVMatrix() { return m_wvMatrix; }

		void update()
		{
			{
				glm::mat<4, 4, T> model( 1 );

				model = glm::translate( model, glm::vec<3, T>{ position.x, position.y, position.z } );
			
				model = glm::rotate<T>( model, glm::radians( rotation.z ), glm::vec<3, T>{ 0, 0, 1 } ); // roll
				model = glm::rotate<T>( model, glm::radians( rotation.y ), glm::vec<3, T>{ 0, 1, 0 } ); // yaw
				model = glm::rotate<T>( model, glm::radians( rotation.x ), glm::vec<3, T>{ 1, 0, 0 } ); // pitch

				model = glm::scale( model, glm::vec<3, T>{ scale.x, scale.y, scale.z } );

				if( parent != nullptr )
					model = parent->getMatrix() * model;

				m_matrix = model;
			}

			{
				cMatrix<T, 4, 4> model{ 1 };
				
				model = Matrix::translate( model, position );
				model = Matrix::scale( model, scale );

				if ( parent != nullptr )
					model = model * parent->getWVMatrix();

				m_wvMatrix = model;
			}
		}

		inline cVector3<T> forward()
		{
			return rotation.eulerToDirection();
		}

///////////////////////////////////////////////////////////////////////////////////////

		Transform<T>* parent = nullptr;
		cVector3<T> position{ 0, 0, 0 };
		cVector3<T> rotation{ 0, 0, 0 };
		cVector3<T> scale   { 1, 1, 1 };

	private:

		cMatrix<T, 4, 4> m_wvMatrix{ 1 };

		glm::mat<4, 4, T> m_matrix{ 1 };
    };

///////////////////////////////////////////////////////////////////////////////////////

	typedef Transform<float>  Transformf;
	typedef Transform<double> Transformd;
	typedef Transform<int>    Transformi;

}