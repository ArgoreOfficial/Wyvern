#pragma once

#include <wv/reflection/reflection.h>

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

	inline void translate( const Vector3f& _translation ) { position += _translation; }
	inline void rotate   ( const Vector3f& _rotation, RotateSpace _space = RotateSpace_local ) { rotation.rotate( _rotation, _space ); }

	inline Matrix4x4f getMatrix() const { return m_matrix; }

	bool update( Transform* _parent, bool _recalculateMatrix = true );

	inline Vector3f forward() const { return rotation.rotateVector( { 0.0f, 0.0f, -1.0f } ); }
	inline Vector3f right()   const { return rotation.rotateVector( { 1.0f, 0.0f,  0.0f } ); }
	inline Vector3f up()      const { return rotation.rotateVector( { 0.0f, 1.0f,  0.0f } ); }

///////////////////////////////////////////////////////////////////////////////////////

	Vector3f position{};
	Rotorf   rotation{};
	Vector3f scale{ 1, 1, 1 };

	Matrix4x4f m_matrix{ 1 };

	static inline wv::Reflection reflection{
		wv::reflect( "pos", &Transform::position ),
		wv::reflect( "rot", &Transform::rotation ),
		wv::reflect( "scl", &Transform::scale )
	};

private:

	Matrix4x4f m_localMatrix{ 1 };

	Vector3f m_cachedPosition{ 0, 0, 0 };
	Rotorf   m_cachedRotation{};
	Vector3f m_cachedScale{ 1, 1, 1 };
};

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

	return _recalculateMatrix || recalc;
}

}