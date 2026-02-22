#pragma once

#include <track_segment.h>

#include <wv/math/vector2.h>
#include <wv/math/vector3.h>
#include <wv/debug/error.h>
#include <wv/memory/memory.h>

class ArcTrackSegment : public ITrackSegment
{
public:
	ArcTrackSegment( const wv::Vector3f& _start, const wv::Vector3f& _centre, double _arc ) :
		m_arcStart{ _start },
		m_arcCentre{ _centre },
		m_arcAngle{ _arc }
	{
		m_arcRadius = ( _start - _centre ).length();
		
		wv::Vector3f centreToStart = ( _start - _centre ).normalized();

		m_baseAngle = wv::Math::angleBetween(
			wv::Vector2f{ -1.0f, 0.0f },
			wv::Vector2f{ centreToStart.x, centreToStart.z },
			true
		);

		wv::Vector3f endPos{
			-std::cosf( m_baseAngle + wv::Math::radians( _arc ) ),
			0.0f,
			std::sinf( m_baseAngle + wv::Math::radians( _arc ) )
		};

		m_arcEnd = _centre + endPos * m_arcRadius;
	}

	virtual ~ArcTrackSegment() { }

	virtual float length() const override {
		// in radians
		double angle = wv::Math::radians( m_arcAngle );

		// arc circumference
		double circm = ( m_arcStart - m_arcCentre ).length() * 2 * wv::Const::Double::PI;

		// length of arc
		double len = ( angle / wv::Math::radians( 360.0 ) ) * circm;

		return std::abs( len );
	}

	virtual wv::Vector3f getStartPosition() const override {
		return m_arcStart;
	}

	virtual wv::Vector3f getEndPosition() const override {
		return m_arcEnd;
	}

	virtual wv::Vector3f getEndRightAngle() const override {
		wv::Vector3f centreToStart = ( m_arcStart - m_arcCentre ).normalized();

		double baseAngle = wv::Math::angleBetween(
			wv::Vector2f{ -1.0f, 0.0f },
			wv::Vector2f{ centreToStart.x, centreToStart.z },
			true
		);

		wv::Vector3f rightAngle{
			-std::cosf( baseAngle + wv::Math::radians( m_arcAngle ) ),
			0.0f,
			std::sinf( baseAngle + wv::Math::radians( m_arcAngle ) )
		};

		if ( m_arcAngle < 0.0 )
			rightAngle *= -1;

		return rightAngle.normalized();
	}

	// 0 <= _t <= 1
	virtual wv::Vector3f getPosition( double _t ) const override 
	{
		_t = wv::Math::clamp( _t, 0.0, 1.0 );

		double arcDistance = m_arcAngle * _t;

		wv::Vector3f centreToStart = ( m_arcStart - m_arcCentre ).normalized();

		wv::Vector3f position{
			-std::cosf( m_baseAngle + wv::Math::radians( arcDistance ) ),
			0.0f,
			std::sinf( m_baseAngle + wv::Math::radians( arcDistance ) )
		};

		return m_arcCentre + position * m_arcRadius;
	}
	
	virtual wv::Vector3f getClosestToPoint( const wv::Vector3f& _point ) const override {
		return getPosition( getClosestTrackPosition( _point ) );
	}

	virtual double getClosestTrackPosition( const wv::Vector3f& _point ) const override {
		wv::Vector3f dirToPoint = _point - m_arcCentre;
		dirToPoint.y = 0.0f;
		dirToPoint.normalize();

		wv::Vector3f circPos = m_arcCentre + dirToPoint * m_arcRadius;

		wv::Vector2f dirToPoint2D = { dirToPoint.x, dirToPoint.z };

		const float radians = wv::Math::angleBetween(
			wv::Vector2f{ -1.0f, 0.0f },
			wv::Vector2f{ dirToPoint.x, dirToPoint.z },
			true
		);

		float arcAngleRadians = wv::Math::radians( m_arcAngle );

		if ( m_arcAngle < 0.0 )
		{
			if ( radians > m_baseAngle )
				return 0.0;

			if ( radians < m_baseAngle + arcAngleRadians )
				return 1.0;

			return ( radians - m_baseAngle ) / arcAngleRadians;
		}
		else
		{
			if ( radians < m_baseAngle )
				return 0.0;

			if ( radians > m_baseAngle + arcAngleRadians )
				return 1.0;

			return ( radians + m_baseAngle ) / arcAngleRadians;
		}
	}

	virtual ITrackSegment* splitSegment( double _t ) override {
		if ( _t <= 0.0 ) return nullptr;
		if ( _t >= 1.0 ) return nullptr;

		// WV_LOG_WARNING( "Add ArcTrackSegment::splitSegment\n" );

		double lowerAngle = ( m_arcAngle * _t );
		double upperArc = m_arcAngle - lowerAngle;
		wv::Vector3f upperStart = getPosition( _t );
		
		ArcTrackSegment* upperSegment = WV_NEW( ArcTrackSegment, upperStart, m_arcCentre, upperArc );
		
		m_arcEnd   = upperStart;
		m_arcAngle = lowerAngle;

		return upperSegment;
	}

private:
	
	double m_arcAngle  = 0.0; // in degrees
	double m_baseAngle = 0.0; // in radians
	double m_arcRadius = 0.0;

	wv::Vector3f m_arcCentre{};
	wv::Vector3f m_arcStart{};
	wv::Vector3f m_arcEnd{};

};
