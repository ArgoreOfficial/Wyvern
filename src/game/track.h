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

	void replaceTrackIndex( size_t _old, size_t _new ) {
		if ( inIndex == _old )
			inIndex = _new;
		else
		{
			for ( size_t i = 0; i < outIndices.size(); i++ )
			{
				if ( outIndices[ i ] == _old )
					outIndices[ i ] = _new;
			}
		}
	}
};

class TrackLength
{
public:
	TrackLength() = default;
	
	void clear();

	void addLineTrack( const wv::Vector3f& _start, const wv::Vector3f& _end );
	void addLineTrack( float _length );
	void addArcTrack( const wv::Vector3f& _start, const wv::Vector3f& _centre, double _arc );
	void addArcTrack( double _radius, double _arc );

	wv::Vector3f getPositionAt( double _trackPosition ) const;
	wv::Vector3f getStartPosition() const { return getPositionAt( 0.0 ); }
	wv::Vector3f getEndPosition()   const { return getPositionAt( m_totalLength ); }

	wv::Vector3f getDirectionAt( double _trackPosition ) const {
		return ( getPositionAt( _trackPosition + 0.1 ) - getPositionAt( _trackPosition ) ).normalized();
	}

	wv::Vector3f getStartDirection() const {
		return getDirectionAt( 0.0 );
	}

	wv::Vector3f getEndDirection() const {
		return getDirectionAt( m_totalLength - 0.1 );
	}

	wv::Vector3f getEndRightAngle() const {
		return m_track.back()->getEndRightAngle();
	}

	wv::Vector3f getClosestToPoint( const wv::Vector3f& _point ) const;
	double getClosestTrackPosition( const wv::Vector3f& _point ) const;
	
	double getSegmentTValue( size_t _segmentIndex, double _trackPosition ) const {
		WV_ASSERT( _segmentIndex < m_track.size() );

		double prevLength = 0.0;
		for ( size_t i = 0; i < _segmentIndex; i++ )
			prevLength += m_track[ i ]->length();

		const double relativePos = _trackPosition - prevLength;
		return relativePos / m_track[ _segmentIndex ]->length();
	}

	int findTrackIndex( double _position ) const;
	bool isPositionInsideTrack( double _position ) const;

	/**
	 * @brief Cut the track length in half
	 * @return The upper half of the track length
	 */
	TrackLength splitTrackAt( double _position );

	double length() const { return m_totalLength; }
	double recalculateLength() {
		m_totalLength = 0.0;
		for ( ITrackSegment* segment : m_track )
			m_totalLength += segment->length();
		return m_totalLength;
	}
	int nextJunctionIndex = -1;
	int prevJunctionIndex = -1;

private:
	double m_totalLength = 0.0;
	std::vector<ITrackSegment*> m_track = {};
};
