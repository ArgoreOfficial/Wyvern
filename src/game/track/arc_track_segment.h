#pragma once

#include <track_segment.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/debug/error.h>

class ArcTrackSegment : public ITrackSegment
{
public:
	ArcTrackSegment( const wv::Vector3f& _start, const wv::Vector3f& _centre, double _arc ) :
		m_arcStart{ _start },
		m_arcCentre{ _centre },
		m_arcAngle{ _arc }
	{
		double radius = ( _start - _centre ).length();
		
		wv::Vector3f centreToStart = ( _start - _centre ).normalized();

		double baseAngle = wv::Math::angleBetween(
			wv::Vector2f{ -1.0f, 0.0f },
			wv::Vector2f{ centreToStart.x, centreToStart.z },
			true
		);

		wv::Vector3f endPos{
			-std::cosf( baseAngle + wv::Math::radians( _arc ) ),
			0.0f,
			std::sinf( baseAngle + wv::Math::radians( _arc ) )
		};

		m_arcEnd = _centre + endPos * radius;
	}

	virtual ~ArcTrackSegment() { }

	virtual float length() const override {
		// in radians
		double angle = wv::Math::radians( m_arcAngle );

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
	virtual wv::Vector3f getPosition( double _t ) const override 
	{
		/*
		// relative to centre
		wv::Vector3f ps = m_arcStart - m_arcCentre;
		wv::Vector3f pe = m_arcEnd - m_arcCentre;

		// flatten
		wv::Vector2f start = { ps.x, ps.z };
		wv::Vector2f end   = { pe.x, pe.z };

		wv::Vector2f slerped = wv::Math::slerp( start, end, _t, true );

		wv::Vector3f pos = { slerped.x, 0.0f, slerped.y };

		return m_arcCentre + pos;
		*/

		_t = wv::Math::clamp( _t, 0.0, 1.0 );

		double arcDistance = m_arcAngle * _t;
		double radius = ( m_arcStart - m_arcCentre ).length();
		wv::Vector3f centreToStart = ( m_arcStart - m_arcCentre ).normalized();

		double baseAngle = wv::Math::angleBetween(
			wv::Vector2f{ -1.0f, 0.0f },
			wv::Vector2f{ centreToStart.x, centreToStart.z },
			true
		);

		wv::Vector3f position{
			-std::cosf( baseAngle + wv::Math::radians( arcDistance ) ),
			0.0f,
			std::sinf( baseAngle + wv::Math::radians( arcDistance ) )
		};

		return m_arcCentre + position * radius;
	}

private:

	double m_arcAngle = 0.0;

	wv::Vector3f m_arcCentre{};
	wv::Vector3f m_arcStart{};
	wv::Vector3f m_arcEnd{};

};
