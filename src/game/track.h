#pragma once

#include "track_segment.h"

#include <wv/math/vector3.h>
#include <vector>

class TrackLength
{
public:
	TrackLength() = default;
	
	void clear();

	void addLineTrack( const wv::Vector3f& _position );
	void addLineTrack( float _length );
	void addArcTrack( double _radius, double _arc );

	wv::Vector3f getPositionAt( double _trackPosition );
	
	int findTrackIndex( double _position );
	bool isPositionInsideTrack( double _position );

	double length() const { return m_totalLength; }

	size_t prevSegmentIndex = 0;
	size_t nextSegmentIndex = 0;

private:
	double m_totalLength = 0.0;
	std::vector<ITrackSegment*> m_track = {};
};
