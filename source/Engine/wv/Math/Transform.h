#pragma once

#include <wv/Math/Vector3.h>
#include <wv/Math/Matrix.h>

#include <vector>

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

		void addChild( Transform<T>* _child );
		void removeChild( Transform<T>* _child );

		bool update( Transform<T>* _parent, bool _recalculateMatrix = true );

		inline cVector3<T> forward() { return rotation.eulerToDirection(); }

///////////////////////////////////////////////////////////////////////////////////////

		cVector3<T> position{ 0, 0, 0 };
		cVector3<T> rotation{ 0, 0, 0 };
		cVector3<T> scale   { 1, 1, 1 };

		Transform<T>* pParent;

		cMatrix<T, 4, 4> m_matrix{ 1 };

	private:

		cMatrix<T, 4, 4> m_localMatrix{ 1 };

		cVector3<T> m_cachedPosition{ 0, 0, 0 };
		cVector3<T> m_cachedRotation{ 0, 0, 0 };
		cVector3<T> m_cachedScale{ 1, 1, 1 };

		std::vector<Transform<T>*> m_children;
    };

///////////////////////////////////////////////////////////////////////////////////////

	typedef Transform<float>  Transformf;
	typedef Transform<double> Transformd;
	typedef Transform<int>    Transformi;

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline void Transform<T>::addChild( Transform<T>* _child )
	{
		if( _child == nullptr )
			return;

		for( auto& child : m_children ) // if child is already added
			if( child == _child ) 
				return;
		
		m_children.push_back( _child );
		_child->pParent = this;
	}

	template<typename T>
	inline void Transform<T>::removeChild( Transform<T>* _child )
	{
		if( _child == nullptr )
			return;

		for( size_t i = 0; i++; i < m_children.size() )
		{
			if( m_children[ i ] != _child )
				continue;

			m_children.erase( m_children.begin() + i );
			return;
		}
	}

	template<typename T>
	inline bool Transform<T>::update( Transform<T>* _parent, bool _recalculateMatrix )
	{

		bool posChanged = position != m_cachedPosition;
		bool rotChanged = rotation != m_cachedRotation;
		bool sclChanged = scale    != m_cachedScale;
		bool recalc = posChanged || rotChanged || sclChanged;

		if ( recalc )
		{
			cMatrix<T, 4, 4> model{ 1 };

			model = Matrix::translate( model, position );

			model = Matrix::rotateZ( model, Math::radians( rotation.z ) );
			model = Matrix::rotateY( model, Math::radians( rotation.y ) );
			model = Matrix::rotateX( model, Math::radians( rotation.x ) );

			model = Matrix::scale( model, scale );

			m_cachedPosition = position;
			m_cachedRotation = rotation;
			m_cachedScale = scale;

			m_localMatrix = model;
		}

		if ( _recalculateMatrix || recalc )
		{
			if( _parent != nullptr )
				m_matrix = m_localMatrix * _parent->getMatrix();
			else
				m_matrix = m_localMatrix;
		}
		else
			m_matrix = m_localMatrix;


		for( auto& child : m_children )
			child->update( this, recalc || _recalculateMatrix );

		return _recalculateMatrix || recalc;
	}

}