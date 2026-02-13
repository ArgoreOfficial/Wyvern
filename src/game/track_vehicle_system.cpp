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
		size_t trackIndex = engine->m_trackIndex;
		double velocity = _ctx.deltaTime * 10.0 * engine->m_throttle;

		engine->m_trackPosition += velocity;
		
		if ( !getTrack( trackIndex ).isPositionInsideTrack( engine->m_trackPosition ) )
		{
			TrackLength& track = getTrack( trackIndex );

			if ( engine->m_trackPosition < 0.0 )
			{
				TrackLength& prevTrack = m_trackLengths[ track.prevSegmentIndex ];

				engine->m_trackPosition = prevTrack.length() + engine->m_trackPosition;
				engine->m_trackIndex = track.prevSegmentIndex;
			}
			else
			{
				engine->m_trackPosition = engine->m_trackPosition - track.length();
				engine->m_trackIndex    = track.nextSegmentIndex;
			}
		}

		wv::Vector3f frontWheelPos = getTrackWorldPosition( engine->m_trackIndex, engine->m_trackPosition + 1.0 );
		wv::Vector3f backWheelPos  = getTrackWorldPosition( engine->m_trackIndex, engine->m_trackPosition - 1.0 );

		wv::Entity* ent = m_engineComponents.getEntity( engine->getID() );

		ent->getTransform().position = ( frontWheelPos + backWheelPos ) / 2.0;
		ent->getTransform().rotation = ( frontWheelPos - backWheelPos ).normalized().directionToEuler();
		ent->getTransform().update( nullptr );

		// if it has changed, update
		engine->m_trackIndex = trackIndex;
	}
}

wv::Vector3f TrackVehicleSystem::getTrackWorldPosition( size_t _index, double _trackPosition )
{
	TrackLength& track = m_trackLengths[ _index ];
	
	if ( track.isPositionInsideTrack( _trackPosition ) )
		return track.getPositionAt( _trackPosition );
	else
	{
		if ( _trackPosition < 0.0 )
		{
			TrackLength& prevTrack = m_trackLengths[ track.prevSegmentIndex ];

			// position on the prev track is the underflow into the previous track length

			return prevTrack.getPositionAt( prevTrack.length() + _trackPosition );
		}
		else
		{
			TrackLength& nextTrack = m_trackLengths[ track.nextSegmentIndex ];

			// position on the next track is the overflow from the current track
			
			return nextTrack.getPositionAt( _trackPosition - track.length() );
		}
	}

	return wv::Vector3f{};
}
