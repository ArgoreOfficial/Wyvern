#pragma once

#include <wv/math/vector3.h>
#include <wv/debug/error.h>

enum class TrackSegmentType
{
	LINE,
	ARC,
	OTHER
};

class ITrackSegment
{
public:
	virtual ~ITrackSegment() { }

	virtual float length() const = 0;

	virtual wv::Vector3f getStartPosition() const = 0;
	virtual wv::Vector3f getEndPosition() const = 0;

	virtual wv::Vector3f getEndRightAngle() const = 0;

	// 0 <= _t <= 1
	virtual wv::Vector3f getPosition( double _t ) const = 0;

	virtual wv::Vector3f getClosestToPoint( const wv::Vector3f& _point ) const = 0;
	virtual double getClosestTrackPosition( const wv::Vector3f& _point ) const = 0;

	virtual ITrackSegment* splitSegment( double _t ) = 0;

	TrackSegmentType getSegmentType() const { return m_segmentType; }

protected:
	TrackSegmentType m_segmentType = TrackSegmentType::OTHER;
};
