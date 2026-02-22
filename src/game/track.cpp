#include "track.h"

#include "track/line_track_segment.h"
#include "track/arc_track_segment.h"

#include <wv/memory/memory.h>
#include <wv/math/vector2.h>

void TrackLength::clear()
{
	for ( ITrackSegment* segment : m_track )
		WV_FREE( segment );

	m_track.clear();
}

void TrackLength::addLineTrack( const wv::Vector3f& _start, const wv::Vector3f& _end )
{
	LineTrackSegment* segment = WV_NEW( LineTrackSegment, _start, _end );

	if ( !m_track.empty() )
	{
		WV_WARNING( "Created line segment with start and end position in track with existing segments\n" );
	}

	m_track.push_back( segment );
	m_totalLength += segment->length();
}

void TrackLength::addLineTrack( float _length )
{
	ITrackSegment* segment{ nullptr };

	if ( !m_track.empty() )
	{
		wv::Vector3f dir = m_track.back()->getEndRightAngle().cross( { 0.0f, 1.0f, 0.0f } );
		segment = WV_NEW( 
			LineTrackSegment,
			m_track.back()->getEndPosition(),
			m_track.back()->getEndPosition() - dir * _length
		);
	}
	else
	{
		segment = WV_NEW( 
			LineTrackSegment, 
			wv::Vector3f{}, 
			wv::Vector3f{ 0.0f, 0.0f, _length } 
		);
	}

	if ( segment )
	{
		m_track.push_back( segment );
		m_totalLength += segment->length();
	}
}

void TrackLength::addArcTrack( const wv::Vector3f& _start, const wv::Vector3f& _centre, double _arc )
{
	ITrackSegment* segment = WV_NEW(
		ArcTrackSegment,
		_start,
		_centre,
		_arc
	);

	if ( segment )
	{
		m_track.push_back( segment );
		m_totalLength += segment->length();
	}
}

void TrackLength::addArcTrack( double _radius, double _arc )
{
	if ( !m_track.empty() )
	{
		ITrackSegment* prev = m_track.back();
		wv::Vector3f cross = prev->getEndRightAngle();

		if ( _arc < 0.0 )
			cross *= -1;
		
		wv::Vector3f centre = prev->getEndPosition() - ( cross * _radius );

		addArcTrack( prev->getEndPosition(), centre, _arc );

	}
	else
	{
		// TODO: this should allow for any position
		wv::Vector3f cross = { 1.0f, 0.0f, 0.0 };
		wv::Vector3f centre = cross * _radius;

		if ( _arc < 0.0 )
		{
			cross *= -1.0;
			_arc *= -1.0;
		}

		addArcTrack( wv::Vector3f{}, centre, _arc );
	}
}

wv::Vector3f TrackLength::getPositionAt( double _trackPosition ) const
{
	WV_ASSERT( m_totalLength > 0.0 );

	int trackIndex = findTrackIndex( _trackPosition );
	
	if ( trackIndex == -1 )
	{
		if ( _trackPosition < 0.0 )
		{
			_trackPosition = 0;
			trackIndex = 0;
		}
		else
		{
			_trackPosition = m_totalLength;
			trackIndex = m_track.size() - 1;
		}
	}

	double prevLength = 0.0;
	for ( size_t i = 0; i < trackIndex; i++ )
		prevLength += m_track[ i ]->length();

	double relativePos = _trackPosition - prevLength;

	ITrackSegment* segment = m_track[ trackIndex ];

	return segment->getPosition( relativePos / segment->length() );
}

wv::Vector3f TrackLength::getClosestToPoint( const wv::Vector3f& _point ) const
{
	float sqrDist = FLT_MAX;
	wv::Vector3f point = _point;

	for ( const ITrackSegment* track : m_track )
	{
		const wv::Vector3f newPoint = track->getClosestToPoint( _point );
		const wv::Vector3f rel = newPoint - _point;
		const float newSqrDist = rel.length();

		if ( newSqrDist < sqrDist )
		{
			sqrDist = newSqrDist;
			point = newPoint;
		}
	}

    return point;
}

double TrackLength::getClosestTrackPosition( const wv::Vector3f& _point ) const
{
	double trackPosition = 0.0;
	float sqrDist = FLT_MAX;
	wv::Vector3f point = _point;
	size_t index = 0;

	for ( size_t i = 0; i < m_track.size(); i++ )
	{
		const ITrackSegment* track = m_track[ i ];
		const double newTrackPosition = track->getClosestTrackPosition( _point );

		const wv::Vector3f newPoint = track->getPosition( newTrackPosition );
		const wv::Vector3f rel = newPoint - _point;
		const float newSqrDist = rel.length();

		if ( newSqrDist < sqrDist )
		{
			sqrDist = newSqrDist;
			point = newPoint;
			index = i;
			trackPosition = newTrackPosition;
		}
	}

	double trueTrackPosition = 0.0;
	for ( size_t i = 0; i < index; i++ )
		trueTrackPosition += m_track[ i ]->length();
	
	trueTrackPosition += m_track[ index ]->length() * trackPosition;

	return trueTrackPosition;
}

int TrackLength::findTrackIndex( double _position ) const
{
	if ( _position < 0.0 )
		return -1;

	double currentLength = 0.0;

	for ( size_t i = 0; i < m_track.size(); i++ )
	{
		double nextLength = currentLength + m_track[ i ]->length();

		if ( _position <= nextLength )
			return i;

		currentLength = nextLength;
	}

	return -1;
}

bool TrackLength::isPositionInsideTrack( double _position ) const
{
	if ( _position < 0 )
		return false;

	if ( _position > m_totalLength )
		return false;

	if ( findTrackIndex( _position ) == -1 )
		return false;

	return true;
}

TrackLength TrackLength::splitTrackAt( double _position )
{
	int segmentToSplit = findTrackIndex( _position );
	if ( segmentToSplit == -1 )
		return {};

	std::vector<ITrackSegment*> lowerTrack = {};

	double segmentT = getSegmentTValue( segmentToSplit, _position );

	ITrackSegment* upperSegment = m_track[ segmentToSplit ]->splitSegment( segmentT );
	if ( upperSegment == nullptr )
		return {};

	for ( size_t i = 0; i <= (size_t)segmentToSplit; i++ )
		lowerTrack.push_back( m_track[ i ] );
	
	TrackLength upperLength{};

	upperLength.m_track.push_back( upperSegment );
	for ( size_t i = segmentToSplit + 1; i < m_track.size(); i++ )
		upperLength.m_track.push_back( m_track[ i ] );
	
	upperLength.recalculateLength();

	m_track = lowerTrack;
	
	recalculateLength();

	return upperLength;
}

