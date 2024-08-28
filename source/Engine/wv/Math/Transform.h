#pragma once
#include <wv/Math/Vector3.h>

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
		
		inline cMatrix<T, 4, 4> getMatrix() { return m_matrix; }

		void update()
		{
			cMatrix<T, 4, 4> model{ 1 };
				
			model = Matrix::translate( model, position );

			model = Matrix::rotateZ( model, Math::radians( rotation.z ) );
			model = Matrix::rotateY( model, Math::radians( rotation.y ) );
			model = Matrix::rotateX( model, Math::radians( rotation.x ) );
				
			model = Matrix::scale( model, scale );

			if ( parent != nullptr )
				model = model * parent->getMatrix();

			m_matrix = model;
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

		cMatrix<T, 4, 4> m_matrix{ 1 };
    };

///////////////////////////////////////////////////////////////////////////////////////

	typedef Transform<float>  Transformf;
	typedef Transform<double> Transformd;
	typedef Transform<int>    Transformi;

}