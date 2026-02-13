#pragma once

#include <track_segment.h>

#include <wv/math/vector3.h>
#include <wv/debug/error.h>

class ArcTrackSegment : public ITrackSegment
{
public:
	ArcTrackSegment( const wv::Vector3f& _start, const wv::Vector3f& _end, const wv::Vector3f& _centre ) :
		m_arcStart{ _start },
		m_arcCentre{ _centre }
	{
		double radius = ( _start - _centre ).length();
		m_arcEnd = _centre + ( _end - _centre ).normalized( radius );
	}

	virtual ~ArcTrackSegment() { }

	virtual float length() const override {
		// angle between start and end
		double angle = wv::Math::angleBetween(
			( m_arcStart - m_arcCentre ).normalized(),
			( m_arcEnd - m_arcCentre ).normalized()
		);

		// arc circumference
		double circm = ( m_arcStart - m_arcCentre ).length() * 2 * wv::Const::Double::PI;

		// length of arc
		double len = ( angle / wv::Math::radians( 360.0 ) ) * circm;

		return len;
	}

	virtual wv::Vector3f getStartPosition() const override {
		return m_arcStart;
	}

	virtual wv::Vector3f getEndPosition() const override {
		return m_arcEnd;
	}

	virtual wv::Vector3f getEndRightAngle() const override {
		return ( m_arcEnd - m_arcCentre ).normalized();
	}

	// 0 <= _t <= 1
	virtual wv::Vector3f getPosition( double _t ) const override {
		return m_arcCentre + wv::Math::slerp(
			m_arcStart - m_arcCentre,
			m_arcEnd - m_arcCentre,
			_t
		);
	}

private:
	wv::Vector3f m_arcCentre{};
	wv::Vector3f m_arcStart{};
	wv::Vector3f m_arcEnd{};

};
