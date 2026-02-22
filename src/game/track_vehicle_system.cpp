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
			TrackPosition trackLoc = getClosestToPoint( rayhit );
			
			wv::Renderer* renderer = wv::getApp()->getRenderer();
			bool withinClickableDistance = ( trackLoc.worldPosition - rayhit ).length() < 1.6f;

			if ( withinClickableDistance )
			{
				renderer->addDebugLine(
					trackLoc.worldPosition,
					trackLoc.worldPosition + wv::Vector3f{ 0.0f, 2.0f, 0.0f } );

				renderer->addDebugLine(
					rayhit,
					trackLoc.worldPosition );

				
				if ( _ctx.inputSystem->getMouseButtonDown( 1 ) )
					beginTrackBuild( trackLoc );
			}

			if ( m_isBuildingTrack )
			{
				renderer->addDebugLine( m_buildingTrackPosition.worldPosition, rayhit );
				TrackPosition connectingTrack{};

				if ( withinClickableDistance )
					connectingTrack = trackLoc;
				else
					connectingTrack.worldPosition = rayhit;

				if ( _ctx.inputSystem->getMouseButtonUp( 1 ) )
					endTrackBuild( connectingTrack );
			}
		}
	}
}

void TrackVehicleSystem::onDebugRender()
{
	auto renderer = wv::getApp()->getRenderer();

	for ( TrackLength& trackLength : m_trackLengths )
	{
		float len = trackLength.length();
		double pos = 0.0;

		if ( len == 0.0f )
			continue;

		for ( size_t i = 0; i < (size_t)len + 1; i++ )
		{
			wv::Vector3f lineStart = trackLength.getPositionAt( pos );
			pos += 1.0;
			wv::Vector3f lineEnd = trackLength.getPositionAt( pos );

			renderer->addDebugLine( lineStart, lineEnd );

			if( trackLength.nextJunctionIndex != -1 )
				renderer->addDebugSphere( trackLength.getEndPosition(), 0.5f );
			else if( trackLength.prevJunctionIndex != -1 )
				renderer->addDebugSphere( trackLength.getStartPosition(), 0.5f );
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

TrackPosition TrackVehicleSystem::getClosestToPoint( const wv::Vector3f& _point ) const
{
	TrackPosition trackPosition{};
	trackPosition.worldPosition = _point;

	float sqrDist = FLT_MAX;

	for ( size_t i = 0; i < m_trackLengths.size(); i++ )
	{
		const TrackLength& trackLength = m_trackLengths[ i ];
	
		const double newTrackPos = trackLength.getClosestTrackPosition( _point );
		const wv::Vector3f newPoint = trackLength.getPositionAt( newTrackPos );

		const wv::Vector3f rel = newPoint - _point;
		const float newSqrDist = rel.length();

		if ( newSqrDist < sqrDist )
		{
			sqrDist = newSqrDist;

			trackPosition.index = i;
			trackPosition.worldPosition = newPoint;
			trackPosition.distanceFromStart = newTrackPos;
		}
	}

	return trackPosition;
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

void TrackVehicleSystem::beginTrackBuild( TrackPosition _trackPosition )
{
	m_isBuildingTrack = true;
	m_buildingTrackPosition = _trackPosition;
}

void TrackVehicleSystem::endTrackBuild( TrackPosition _connectTrackPosition )
{
	m_isBuildingTrack = false;

	wv::Debug::Print( 
		"Track %i at %f, %s\n", 
		m_buildingTrackPosition.index, 
		m_buildingTrackPosition.distanceFromStart, 
		std::format( "{}", m_buildingTrackPosition.worldPosition ).c_str() 
	);

	int newJunctionIndex = (int)m_trackJunctions.size();
	
	if ( !splitTrackLength( m_buildingTrackPosition.index, m_buildingTrackPosition.distanceFromStart ) )
		return; // TODO

	TrackJunction& junction = m_trackJunctions.back();
	junction.outIndices.push_back( m_trackLengths.size() );
	junction.currentTrackIndex++;

	// Create new connecting track

	TrackLength& trackLength = m_trackLengths[ m_buildingTrackPosition.index ];
	
	TrackLength newTrack{};
	newTrack.addLineTrack( trackLength.getEndPosition(), _connectTrackPosition.worldPosition );

	if ( _connectTrackPosition.index != -1 )
	{
		TrackJunction connectingJunction{};
		connectingJunction.inIndex = m_trackLengths.size(); // index of new track
		connectingJunction.outIndices.push_back( _connectTrackPosition.index );
		newTrack.nextJunctionIndex = m_trackJunctions.size();

		m_trackJunctions.push_back( connectingJunction );
	}

	newTrack.prevJunctionIndex = newJunctionIndex;
	
	m_trackLengths.push_back( newTrack );

}

bool TrackVehicleSystem::splitTrackLength( size_t _trackIndex, double _trackPosition )
{
	int newJunctionIndex = (int)m_trackJunctions.size();

	// new junction between the split lengths
	TrackJunction junction{};
	junction.inIndex = m_buildingTrackPosition.index;

	// Split and add upper track segment
	
	TrackLength& trackLength = m_trackLengths[ m_buildingTrackPosition.index ];
	TrackLength upperTrack = trackLength.splitTrackAt( m_buildingTrackPosition.distanceFromStart );

	if ( upperTrack.length() <= 0.0 )
		return false; // no spltting occured, we're at the edge of the track length

	// make sure any connecting junction on the upper half gets the new index
	if ( trackLength.nextJunctionIndex != -1 )
	{
		upperTrack.nextJunctionIndex = trackLength.nextJunctionIndex;

		TrackJunction& oldJunction = m_trackJunctions[ trackLength.nextJunctionIndex ];
		oldJunction.replaceTrackIndex( m_buildingTrackPosition.index, m_trackLengths.size() );
	}

	trackLength.nextJunctionIndex = newJunctionIndex;
	upperTrack.prevJunctionIndex = newJunctionIndex;

	junction.outIndices.push_back( m_trackLengths.size() ); // index of the new track
	m_trackLengths.push_back( upperTrack );

	m_trackJunctions.push_back( junction );

	return true;
}
