#include "track_component.h"

void TrackComponent::cullInvalidIndices()
{
	// check for invalid junction indices
	for ( size_t i = 0; i < m_trackLengths.size(); i++ )
	{
		TrackLength* trackLength = m_trackLengths[ i ];

		if ( trackLength->nextJunctionIndex >= (int)m_trackJunctions.size() || trackLength->nextJunctionIndex < -1 )
		{
			WV_LOG_WARNING( "Track length %i had invalid nextJunctionIndex %i\n", (int)i, trackLength->nextJunctionIndex );
			trackLength->nextJunctionIndex = -1;
		}

		if ( trackLength->prevJunctionIndex >= (int)m_trackJunctions.size() || trackLength->prevJunctionIndex < -1 )
		{
			WV_LOG_WARNING( "Track length %i had invalid prevJunctionIndex %i\n", (int)i, trackLength->prevJunctionIndex );
			trackLength->prevJunctionIndex = -1;
		}
	}

	for ( size_t i = 0; i < m_trackJunctions.size(); i++ )
	{
		TrackJunction* trackJunction = m_trackJunctions[ i ];

		if ( trackJunction->inIndex >= (int)m_trackLengths.size() )
		{
			WV_LOG_WARNING( "Track Junction %i had invalid inIndex %i\n", (int)i, trackJunction->inIndex );
			trackJunction->inIndex = -1;
		}
	}
}

