#pragma once

#include "track_segment.h"

#include <wv/math/vector3.h>
#include <vector>

struct TrackJunction
{
	size_t inIndex = 0;

	std::vector<size_t> outIndices{};
	size_t currentTrackIndex = 0;

	int getConnectedTrack( size_t _track )
	{
		if ( inIndex == _track ) // coming from "in" direction
		{
			if ( outIndices.empty() ) // no tracks
				return -1;
			else
				return outIndices[ currentTrackIndex ];
		}
		else
			return inIndex;
	}

	bool hasTrackIndex( size_t _index ) const {
		if ( inIndex == _index )
			return true;

		for ( size_t index : outIndices )
			if ( index == _index )
				return true;
		
		return false;
	}
};

class TrackLength
{
public:
	TrackLength() = default;
	
	void clear();

	void addLineTrack( const wv::Vector3f& _start, const wv::Vector3f& _end );
	void addLineTrack( float _length );
	void addArcTrack( double _radius, double _arc );

	wv::Vector3f getPositionAt( double _trackPosition ) const;
	wv::Vector3f getStartPosition() const { return getPositionAt( 0.0 ); }
	wv::Vector3f getEndPosition()   const { return getPositionAt( m_totalLength ); }

	wv::Vector3f getClosestToPoint( const wv::Vector3f& _point ) const;
	double getClosestTrackPosition( const wv::Vector3f& _point ) const;

	int findTrackIndex( double _position ) const;
	bool isPositionInsideTrack( double _position ) const;

	double length() const { return m_totalLength; }

	int nextJunctionIndex = -1;
	int prevJunctionIndex = -1;

private:
	double m_totalLength = 0.0;
	std::vector<ITrackSegment*> m_track = {};
};
