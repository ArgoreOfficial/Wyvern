#pragma once

#include <track_segment.h>

#include <wv/math/geometry.h>

#include <wv/debug/error.h>
#include <wv/memory/memory.h>

class LineTrackSegment : public ITrackSegment
{
public:
	LineTrackSegment( const wv::Vector3f& _start, const wv::Vector3f& _end ) :
		m_lineStart{ _start },
		m_lineEnd{ _end }
	{
	}

	virtual ~LineTrackSegment() { }

	virtual float length() const override {
		return ( m_lineEnd - m_lineStart ).length();
	}

	virtual wv::Vector3f getStartPosition() const override {
		return m_lineStart;
	}

	virtual wv::Vector3f getEndPosition() const override {
		return m_lineEnd;
	}

	virtual wv::Vector3f getEndRightAngle() const override {
		return ( m_lineEnd - m_lineStart ).normalized().cross( { 0.0f, 1.0f, 0.0f } );
	}

	// 0 <= _t <= 1
	virtual wv::Vector3f getPosition( double _t ) const override {
		return wv::Math::lerp( m_lineStart, m_lineEnd, _t );
	}

	virtual wv::Vector3f getClosestToPoint( const wv::Vector3f& _point ) const override {
		return getPosition( getClosestTrackPosition( _point ) );
	}
	
	virtual double getClosestTrackPosition( const wv::Vector3f& _point ) const override {
		// copied from wv::Math::lineClosestPointClamped

		wv::Vector3f d = m_lineEnd - m_lineStart;
		const double lineLength = d.length();

		d.normalize();

		const const wv::Vector3f v = _point - m_lineStart;
		const double dotProd = v.dot( d );

		return wv::Math::max( dotProd, 0.0 ) / lineLength;
	}

	/**
	 * @brief Split the segment in half
	 * @param _t position on segment, 0 to 1
	 * @return The upper segment
	 */
	virtual ITrackSegment* splitSegment( double _t ) override {
		if ( _t <= 0.0 ) return nullptr;
		if ( _t >= 1.0 ) return nullptr;
			
		const double len = length();
		wv::Vector3f dir = ( m_lineEnd - m_lineStart ).normalized();
	
		wv::Vector3f lowerEnd = m_lineStart + ( dir * _t * len );
		wv::Vector3f upperStart = lowerEnd;

		LineTrackSegment* upperSegment = WV_NEW( LineTrackSegment, upperStart, m_lineEnd );
		
		m_lineEnd = lowerEnd;

		return upperSegment;
	}

private:
	wv::Vector3f m_lineStart{};
	wv::Vector3f m_lineEnd{};

};
