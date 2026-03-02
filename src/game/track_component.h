#pragma once

#include <wv/entity/entity_component.h>
#include <wv/memory/memory.h>

#include "track.h"
#include "track/arc_track_segment.h"

class TrackComponent : public wv::IEntityComponent
{
	WV_REFLECT_TYPE( TrackComponent, wv::IEntityComponent )
public:

	TrackLength* createTrackLength( size_t* _outIndex = nullptr ) {
		if ( _outIndex )
			*_outIndex = m_trackLengths.size();

		TrackLength* trackLength = WV_NEW( TrackLength );
		m_trackLengths.push_back( trackLength );
		return trackLength;
	}

	TrackLength* createTrackLength( const TrackLength& _copy, size_t* _outIndex = nullptr ) {
		if ( _outIndex )
			*_outIndex = m_trackLengths.size();

		TrackLength* trackLength = WV_NEW( TrackLength );
		*trackLength = _copy;
		m_trackLengths.push_back( trackLength );
		return trackLength;
	}

	TrackJunction* createTrackJunction( size_t* _outIndex = nullptr ) {
		if ( _outIndex )
			*_outIndex = m_trackJunctions.size();

		TrackJunction* junction = WV_NEW( TrackJunction );
		m_trackJunctions.push_back( junction );
		return junction;
	}

	const std::vector<TrackLength*>& getTrackLengths() const { return m_trackLengths; }
	TrackLength* getTrack( size_t _index ) { return m_trackLengths[ _index ]; }

	void cullInvalidIndices();

	std::vector<TrackLength*>   m_trackLengths{};
	std::vector<TrackJunction*> m_trackJunctions{};

	bool m_requiresInvalidCheck = true;

protected:
};
