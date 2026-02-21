#include "track_vehicle_system.h"

#include <wv/application.h>
#include <wv/rendering/renderer.h>

#include <wv/entity/entity.h>
#include <wv/entity/entity_component.h>
#include <wv/entity/world.h>
#include <wv/rendering/viewport.h>
#include <wv/camera/view_volume.h>

#include <wv/math/geometry.h>

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
		
		bool playJointSound = trackLocation.first != engine->m_trackIndex;
		// if ( playJointSound )
		// 	wv::Debug::Print( "Clunk\n" );

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

	// Line test
	{
		wv::Vector2i mousePos = _ctx.inputSystem->getMousePosition();
		wv::Vector3f p1 = _ctx.viewport->getViewVolume()->screenToWorld( mousePos.x, mousePos.y, 0.0f );
		wv::Vector3f p2 = _ctx.viewport->getViewVolume()->screenToWorld( mousePos.x, mousePos.y, 0.1f );

		wv::Vector3f raydir = ( p2 - p1 ).normalized();
		wv::Vector3f rayhit{};

		if ( wv::Math::linePlaneIntersection( p1, raydir, {}, wv::Vector3f::up(), &rayhit ) )
		{
			auto trackLoc = getClosestToPoint( rayhit );
			
			if ( ( trackLoc.second - rayhit ).length() < 1.6f )
			{
				auto renderer = wv::getApp()->getRenderer();
				renderer->addDebugLine(
					trackLoc.second, 
					trackLoc.second + wv::Vector3f{ 0.0f, 2.0f, 0.0f } );

				renderer->addDebugLine(
					rayhit,
					trackLoc.second );
				
			}

		}
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

std::pair<int, wv::Vector3f> TrackVehicleSystem::getClosestToPoint( const wv::Vector3f& _point ) const
{
	int idx = -1;
	wv::Vector3f point = _point;
	float sqrDist = FLT_MAX;

	for ( size_t i = 0; i < m_trackLengths.size(); i++ )
	{
		const TrackLength& trackLength = m_trackLengths[ i ];
	
		const wv::Vector3f newPoint = trackLength.getClosestToPoint( _point );
		const wv::Vector3f rel = newPoint - _point;
		const float newSqrDist = rel.length();

		if ( newSqrDist < sqrDist )
		{
			sqrDist = newSqrDist;
			idx = i;
			point = newPoint;
		}
	}

	return { idx, point };
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
