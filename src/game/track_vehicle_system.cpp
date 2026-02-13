#include "track_vehicle_system.h"

#include <wv/entity/entity.h>
#include <wv/entity/entity_component.h>
#include <wv/entity/world.h>

void TrackVehicleSystem::initialize()
{
	
}

void TrackVehicleSystem::shutdown()
{
	for ( TrackLength& trackLength : m_trackLengths )
		trackLength.clear();

	m_trackLengths.clear();
}

void TrackVehicleSystem::registerComponent( wv::Entity* _entity, wv::IEntityComponent* _component )
{
	if ( TrackEngineComponent* engine = wv::tryCast<TrackEngineComponent>( _component ) )
	{
		m_engineComponents.registerComponent( _entity, _component );
	}
	else if ( TrackVehicleComponent* vehicle = wv::tryCast<TrackVehicleComponent>( _component ) )
	{
		m_vehicleComponents.push_back( vehicle );
	}
}

void TrackVehicleSystem::unregisterComponent( wv::Entity* _entity, wv::IEntityComponent* _component )
{
	if ( TrackEngineComponent* engine = wv::tryCast<TrackEngineComponent>( _component ) )
	{
		m_engineComponents.unregisterComponent( _entity, _component );
	}
	else if ( TrackVehicleComponent* vehicle = wv::tryCast<TrackVehicleComponent>( _component ) )
	{
		std::erase( m_vehicleComponents, vehicle );
	}
}

void TrackVehicleSystem::update( wv::WorldUpdateContext& _ctx )
{
	if ( m_trackLengths.empty() )
		return;

	wv::Vector3f teststart = m_trackLengths[ 0 ].getPositionAt( 0 );
	wv::Vector3f testend = m_trackLengths[ 0 ].getPositionAt( m_trackLengths[ 0 ].length() );

	for ( TrackEngineComponent* engine : m_engineComponents.getComponents() )
	{
		double velocity = _ctx.deltaTime * 10.0 * engine->m_throttle;

		engine->m_trackPosition += velocity;

		std::pair<int, double> trackLocation = moveAlongTrack( engine->m_trackIndex, engine->m_trackPosition );
		if ( trackLocation.first < 0 ) 
			continue; // error
		
		engine->m_trackIndex    = trackLocation.first;
		engine->m_trackPosition = trackLocation.second;

		wv::Vector3f frontWheelPos = getTrackWorldPosition( engine->m_trackIndex, engine->m_trackPosition + 1.0 );
		wv::Vector3f backWheelPos  = getTrackWorldPosition( engine->m_trackIndex, engine->m_trackPosition - 1.0 );

		wv::Entity* ent = m_engineComponents.getEntity( engine->getID() );

		ent->getTransform().position = ( frontWheelPos + backWheelPos ) / 2.0;
		//ent->getTransform().position = getTrackWorldPosition( engine->m_trackIndex, engine->m_trackPosition );

		ent->getTransform().rotation = ( frontWheelPos - backWheelPos ).normalized().directionToEuler();
		ent->getTransform().update( nullptr );
	}
}

wv::Vector3f TrackVehicleSystem::getTrackWorldPosition( size_t _track, double _position )
{
	std::pair<int, double> trackLocation = moveAlongTrack( _track, _position );

	if ( trackLocation.first == -1 )
		return {};

	TrackLength& track = m_trackLengths[ trackLocation.first ];
	return track.getPositionAt( trackLocation.second );
}

std::pair<int, double> TrackVehicleSystem::moveAlongTrack( size_t _track, double _movedPosition )
{
	if ( _track < 0 )
		return { _track, _movedPosition };

	if ( getTrack( _track ).isPositionInsideTrack( _movedPosition ) )
	{
		// still inside of track, no need to check for junctions

		return { _track, _movedPosition };
	}

	// not inside track, check for junctions
	
	TrackLength& track = getTrack( _track );

	if ( _movedPosition < 0.0 )
	{
		if ( track.prevJunctionIndex >= 0 && track.prevJunctionIndex < m_trackJunctions.size() )
		{
			TrackJunction& junction = m_trackJunctions[ track.prevJunctionIndex ];
			int newTrackIndex = junction.getConnectedTrack( _track );
			
			// junction leads to new track
			if ( newTrackIndex != -1 )
			{
				TrackLength& newTrack = m_trackLengths[ newTrackIndex ];

				return { newTrackIndex, newTrack.length() + _movedPosition };
			}
		}
		
		// no junction, or junction did not lead anywhere
		return { _track, 0.0 };
	}
	else
	{
		if ( track.nextJunctionIndex >= 0 && track.nextJunctionIndex < m_trackJunctions.size() )
		{
			TrackJunction& junction = m_trackJunctions[ track.nextJunctionIndex ];
			int newTrackIndex = junction.getConnectedTrack( _track );

			// junction leads to new track
			if ( newTrackIndex != -1 )
			{
				TrackLength& newTrack = m_trackLengths[ newTrackIndex ];

				return { newTrackIndex, _movedPosition - track.length() };
			}
		}
		
		// no junction, or junction did not lead anywhere
		return { _track, track.length() };
	}
}
