#pragma once
#include <wv/Math/Vector3.h>

// TODO: change to wv/Math/Matrix
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv 
{

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
    class cTransform
    {
	public:

		void setPosition( wv::cVector3<T> _position ) { position = _position; }
		void setRotation( wv::cVector3<T> _rotation ) { rotation = _rotation; }
		void setScale   ( wv::cVector3<T> _scale )    { scale = _scale; }
		
		void translate( wv::cVector3<T> _translation ) { position += _translation; }
		void rotate   ( wv::cVector3<T> _rotation )    { rotation += _rotation; }
		
		inline glm::mat<4, 4, T> getMatrix( void )
		{
			glm::mat<4,4,T> model( 1 );
			
			model = glm::translate( model, glm::vec<3, T>{ position.x, position.y, position.z } );
			model = glm::scale( model, glm::vec<3, T>{ scale.x, scale.y, scale.z } );
			
			model = glm::rotate<T>( model, glm::radians( rotation.x ), glm::vec<3, T>{ 1, 0, 0 } );
			model = glm::rotate<T>( model, glm::radians( rotation.y ), glm::vec<3, T>{ 0, 1, 0 } );
			model = glm::rotate<T>( model, glm::radians( rotation.z ), glm::vec<3, T>{ 0, 0, 1 } );
			
			return model;
		}

///////////////////////////////////////////////////////////////////////////////////////
		
		cVector3<T> position{ 0, 0, 0 };
		cVector3<T> rotation{ 0, 0, 0 };
		cVector3<T> scale   { 1, 1, 1 };

    };

///////////////////////////////////////////////////////////////////////////////////////

	typedef cTransform<float>  cTransformf;
	typedef cTransform<double> cTransformd;
	typedef cTransform<int>    cTransformi;

}