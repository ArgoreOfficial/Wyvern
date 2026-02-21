#pragma once

#include <track_segment.h>

#include <wv/math/geometry.h>

#include <wv/debug/error.h>

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
		return wv::Math::lineClosestPointClamped( m_lineStart, m_lineEnd, _point );
	}

private:
	wv::Vector3f m_lineStart{};
	wv::Vector3f m_lineEnd{};

};
