#pragma once

#include <wv/Math/Vector3.h>
#include <wv/Math/Matrix.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty>
class Transform
{

public:

	inline void setPosition( const wv::Vector3<_Ty>& _position ) { position = _position; }
	inline void setRotation( const wv::Vector3<_Ty>& _rotation ) { rotation = _rotation; }
	inline void setScale( const wv::Vector3<_Ty>& _scale ) { scale = _scale; }

	inline void setPositionRotation( const wv::Vector3<_Ty>& _position, const wv::Vector3<_Ty>& _rotation ) {
		position = _position;
		rotation = _rotation;
	}

	inline void translate( wv::Vector3<_Ty> _translation ) { position += _translation; }
	inline void rotate( wv::Vector3<_Ty> _rotation ) { rotation += _rotation; }

	inline Matrix<_Ty, 4, 4> getMatrix() { return m_matrix; }

	void addChild( Transform<_Ty>* _child );
	void removeChild( Transform<_Ty>* _child );

	bool update( Transform<_Ty>* _parent, bool _recalculateMatrix = true );

	inline Vector3<_Ty> forward() { return rotation.eulerToDirection(); }

///////////////////////////////////////////////////////////////////////////////////////

	Vector3<_Ty> position{ 0, 0, 0 };
	Vector3<_Ty> rotation{ 0, 0, 0 };
	Vector3<_Ty> scale{ 1, 1, 1 };

	Transform<_Ty>* pParent = nullptr;

	Matrix<_Ty, 4, 4> m_matrix{ 1 };

private:

	Matrix<_Ty, 4, 4> m_localMatrix{ 1 };

	Vector3<_Ty> m_cachedPosition{ 0, 0, 0 };
	Vector3<_Ty> m_cachedRotation{ 0, 0, 0 };
	Vector3<_Ty> m_cachedScale{ 1, 1, 1 };

	std::vector<Transform<_Ty>*> m_children;
};

///////////////////////////////////////////////////////////////////////////////////////

typedef Transform<float>  Transformf;
typedef Transform<double> Transformd;
typedef Transform<int>    Transformi;

///////////////////////////////////////////////////////////////////////////////////////

template<typename _Ty>
inline void Transform<_Ty>::addChild( Transform<_Ty>* _child )
{
	if ( _child == nullptr )
		return;

	for ( auto& child : m_children ) // if child is already added
		if ( child == _child )
			return;

	m_children.push_back( _child );
	_child->pParent = this;
}

template<typename _Ty>
inline void Transform<_Ty>::removeChild( Transform<_Ty>* _child )
{
	if ( _child == nullptr )
		return;

	for ( size_t i = 0; i++; i < m_children.size() )
	{
		if ( m_children[ i ] != _child )
			continue;

		m_children.erase( m_children.begin() + i );
		return;
	}
}

template<typename _Ty>
inline bool Transform<_Ty>::update( Transform<_Ty>* _parent, bool _recalculateMatrix )
{

	bool posChanged = position != m_cachedPosition;
	bool rotChanged = rotation != m_cachedRotation;
	bool sclChanged = scale != m_cachedScale;
	bool recalc = posChanged || rotChanged || sclChanged;

	if ( recalc )
	{
		Matrix<_Ty, 4, 4> model{ 1 };

		model = MatrixUtil::translate( model, position );

		model = MatrixUtil::rotateZ( model, Math::radians( rotation.z ) );
		model = MatrixUtil::rotateY( model, Math::radians( rotation.y ) );
		model = MatrixUtil::rotateX( model, Math::radians( rotation.x ) );

		model = MatrixUtil::scale( model, scale );

		m_cachedPosition = position;
		m_cachedRotation = rotation;
		m_cachedScale = scale;

		m_localMatrix = model;
	}

	if ( _recalculateMatrix || recalc )
	{
		if ( _parent != nullptr )
			m_matrix = m_localMatrix * _parent->getMatrix();
		else
			m_matrix = m_localMatrix;
	}
	else
		m_matrix = m_localMatrix;


	for ( auto& child : m_children )
		child->update( this, recalc || _recalculateMatrix );

	return _recalculateMatrix || recalc;
}

}