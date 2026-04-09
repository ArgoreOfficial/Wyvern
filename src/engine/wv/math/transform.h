#pragma once

#include <wv/math/vector3.h>
#include <wv/math/matrix.h>
#include <wv/math/rotor.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

class Transform
{

public:
	Transform() = default;

	inline void setPosition( const wv::Vector3f& _position ) { position = _position; }
	inline void setScale( const wv::Vector3f& _scale ) { scale = _scale; }

	inline void translate( wv::Vector3f _translation ) { position += _translation; }
	//inline void rotate( wv::Vector3<_Ty> _rotation ) { rotation += _rotation; }

	inline Matrix4x4f getMatrix() const { return m_matrix; }

	void addChild( Transform* _child );
	void removeChild( Transform* _child );

	bool update( Transform* _parent, bool _recalculateMatrix = true );

	inline Vector3f forward() const { return rotation.rotateVector( { 0.0f, 0.0f, -1.0f } ); }
	inline Vector3f right()   const { return rotation.rotateVector( { 1.0f, 0.0f,  0.0f } ); }
	inline Vector3f up()      const { return rotation.rotateVector( { 0.0f, 1.0f,  0.0f } ); }

///////////////////////////////////////////////////////////////////////////////////////

	Vector3f position{};
	Rotorf   rotation{};
	Vector3f scale{ 1, 1, 1 };

	Transform* pParent = nullptr;

	Matrix4x4f m_matrix{ 1 };

private:

	Matrix4x4f m_localMatrix{ 1 };

	Vector3f m_cachedPosition{ 0, 0, 0 };
	Rotorf   m_cachedRotation{};
	Vector3f m_cachedScale{ 1, 1, 1 };

	std::vector<Transform*> m_children;
};

///////////////////////////////////////////////////////////////////////////////////////

inline void Transform::addChild( Transform* _child )
{
	if ( _child == nullptr )
		return;

	for ( auto& child : m_children ) // if child is already added
		if ( child == _child )
			return;

	m_children.push_back( _child );
	_child->pParent = this;
}

inline void Transform::removeChild( Transform* _child )
{
	if ( _child == nullptr )
		return;

	for ( size_t i = 0; i < m_children.size(); i++ )
	{
		if ( m_children[ i ] != _child )
			continue;

		m_children.erase( m_children.begin() + i );
		return;
	}
}

inline bool Transform::update( Transform* _parent, bool _recalculateMatrix )
{

	bool posChanged = position != m_cachedPosition;
	bool rotChanged = rotation != m_cachedRotation;
	bool sclChanged = scale != m_cachedScale;
	bool recalc = posChanged || rotChanged || sclChanged;

	if ( recalc )
	{
		Matrix4x4f model{ 1 };

		model = Math::translate( model, position );
		model = rotation.toMatrix4x4() * model;
		model = Math::scale( model, scale );

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