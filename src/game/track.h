#pragma once

#include "track_segment.h"

#include <wv/math/vector3.h>
#include <vector>

struct Track
{
	void addLineTrack( const wv::Vector3f& _position )
	{
		if ( !m_track.empty() )
			m_track.push_back( TrackSegment::createLineTrack( m_track.back().getEndPosition(), _position ) );
		else
			m_track.push_back( TrackSegment::createLineTrack( {}, _position ) );
	}
	
	void addLineTrack( float _length )
	{
		if ( !m_track.empty() )
		{
			wv::Vector3f dir = m_track.back().getEndRightAngle().cross( { 0.0f, 1.0f, 0.0f } );
			TrackSegment segment = TrackSegment::createLineTrack(
				m_track.back().getEndPosition(), 
				m_track.back().getEndPosition() - dir * _length 
			);
			m_track.push_back( segment );
		}
		else
			m_track.push_back( TrackSegment::createLineTrack( {}, { 0.0f, 0.0f, _length } ) );
	}

	void addArcTrack( double _radius, double _arc )
	{
		if ( !m_track.empty() )
		{
			TrackSegment prev   = m_track.back();
			wv::Vector3f cross  = prev.getEndRightAngle();
			
			if ( _arc < 0.0 )
			{
				cross *= -1.0;
				_arc  *= -1.0;
			}

			
			wv::Vector3f centre = prev.getEndPosition() - ( cross * _radius );

			wv::Vector3f vecA = { 1.0f, 0.0f, 0.0f };
			wv::Vector3f vecB = ( prev.getEndPosition() - centre ).normalized();
			double baseAngle = wv::Math::angleBetween( vecA, vecB );
			
			wv::Vector3f endPos{
				(float)std::cos( baseAngle + wv::Math::radians( _arc ) ),
				0.0f,
				(float)std::sin( baseAngle + wv::Math::radians( _arc ) )
			};

			TrackSegment segment = TrackSegment::createArcTrack( 
				prev.getEndPosition(), 
				centre + endPos * _radius, 
				centre 
			);

			m_track.push_back( segment );
		}
		else
		{
			// TODO: this should allow for any position
			wv::Vector3f cross  = { 1.0f, 0.0f, 0.0 };
			wv::Vector3f centre = cross * _radius;

			wv::Vector3f endPos{
				std::cosf( wv::Math::radians( _arc ) ),
				0.0f,
				std::sinf( wv::Math::radians( _arc ) )
			};

			TrackSegment segment = TrackSegment::createArcTrack(
				{},
				endPos * _radius,
				centre
			);

			m_track.push_back( segment );
		}

	}

	wv::Vector3f getPositionAt( double _trackPosition )
	{
		size_t trackIndex = findTrackIndex( _trackPosition );
		if ( trackIndex == -1 )
		{
			_trackPosition = 0;
			trackIndex = 0;
		}

		double prevLength = 0.0;
		for ( size_t i = 0; i < trackIndex; i++ )
			prevLength += m_track[ i ].length();

		double relativePos = _trackPosition - prevLength;

		TrackSegment segment = m_track[ trackIndex ];

		return segment.getPosition( relativePos / segment.length() );
	}

	int findTrackIndex( double _position ) {
		double currentLength = 0.0;

		for ( size_t i = 0; i < m_track.size(); i++ )
		{
			double nextLength = currentLength + m_track[ i ].length();

			if ( _position <= nextLength )
				return i;

			currentLength = nextLength;
		}

		return -1;
	}

	bool isPositionInsideTrack( double _position ) {
		if ( _position < 0 )
			return false;

		if ( findTrackIndex( _position ) == -1 )
			return false;

		return true;
	}

	std::vector<TrackSegment> m_track = {};
};
