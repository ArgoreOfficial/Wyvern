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

	TrackLength* track = &m_trackLengths[ 0 ];

	for ( TrackEngineComponent* engine : m_engineComponents.getComponents() )
	{
		engine->m_trackPosition = engine->m_trackPosition + _ctx.deltaTime * 50.0 * engine->m_throttle;
		
		if ( !track->isPositionInsideTrack( engine->m_trackPosition ) )
		{
			if ( engine->m_trackPosition < 0.0 )
			{
				TrackLength& prevTrack = m_trackLengths[ track->prevSegmentIndex ];
				engine->m_trackPosition = prevTrack.length() - engine->m_trackPosition;
			}
			else
			{
				engine->m_trackPosition = engine->m_trackPosition - track->length();
			}
		}

		wv::Vector3f frontWheelPos = getTrackWorldPosition( 0, engine->m_trackPosition + 1.0 );
		wv::Vector3f backWheelPos  = getTrackWorldPosition( 0, engine->m_trackPosition - 1.0 );

		wv::Entity* ent = m_engineComponents.getEntity( engine->getID() );

		ent->getTransform().position = ( frontWheelPos + backWheelPos ) / 2.0;
		ent->getTransform().rotation = ( frontWheelPos - backWheelPos ).normalized().directionToEuler();
		ent->getTransform().update( nullptr );

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

			return prevTrack.getPositionAt( prevTrack.length() - _trackPosition );
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
