#pragma once

#include <wv/math/vector3.h>
#include <wv/debug/error.h>

class TrackSegment
{
public:
	static TrackSegment createLineTrack( const wv::Vector3f& _start, const wv::Vector3f& _end )
	{
		TrackSegment segment{};
		segment.m_segmentType = TrackSegment::LINE;
		segment.m_lineStart   = _start;
		segment.m_lineEnd     = _end;
		return segment;
	}

	static TrackSegment createArcTrack( const wv::Vector3f& _start, const wv::Vector3f& _end, const wv::Vector3f& _centre )
	{
		double radius = ( _start - _centre ).length();

		TrackSegment segment{};
		segment.m_segmentType = TrackSegment::ARC;
		segment.m_arcStart    = _start;
		segment.m_arcCentre   = _centre;
		segment.m_arcEnd      = _centre + ( _end - _centre ).normalized( radius );
		return segment;
	}

	float length() const {
		if ( m_segmentType == LINE )
			return ( m_lineEnd - m_lineStart ).length();
		else
		{
			// angle between start and end
			double angle = wv::Math::angleBetween( 
				( m_arcStart - m_arcCentre ).normalized(),
				( m_arcEnd   - m_arcCentre ).normalized()
			);

			// arc circumference
			double circm = ( m_arcStart - m_arcCentre ).length() * 2 * wv::Const::Double::PI;

			// length of arc
			double len = ( angle / wv::Math::radians( 360.0 ) ) * circm;

			return len;
		}
			
	}

	wv::Vector3f getStartPosition() const {
		if ( m_segmentType == LINE )
			return m_lineStart;
		else
			return m_arcStart;
	}

	wv::Vector3f getEndPosition() const {
		if ( m_segmentType == LINE )
			return m_lineEnd;
		else
			return m_arcEnd;
	}

	wv::Vector3f getEndRightAngle() const {
		if ( m_segmentType == LINE )
			return ( m_lineStart - m_lineEnd ).normalized().cross( { 0.0f, 1.0f, 0.0f } );
		else
			return ( m_arcEnd - m_arcCentre ).normalized();
	}

	// _t = 0 to 1
	wv::Vector3f getPosition( double _t ) const {
		if ( m_segmentType == LINE )
			return wv::Math::lerp( m_lineStart, m_lineEnd, _t );
		else
		{
			return m_arcCentre + wv::Math::slerp(
				m_arcStart - m_arcCentre, 
				m_arcEnd   - m_arcCentre, 
				_t 
			);
		}
	}

private:
	enum Type
	{
		LINE,
		ARC
	} m_segmentType = LINE;

	// Line track segment

	wv::Vector3f m_lineStart{};
	wv::Vector3f m_lineEnd{};

	// Arc track segment
	wv::Vector3f m_arcCentre{};
	wv::Vector3f m_arcStart{};
	wv::Vector3f m_arcEnd{};

};
