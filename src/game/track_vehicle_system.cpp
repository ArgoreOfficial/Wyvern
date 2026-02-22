#include "track_vehicle_system.h"

#include <wv/application.h>
#include <wv/rendering/renderer.h>

#include <wv/entity/entity.h>
#include <wv/entity/entity_component.h>
#include <wv/entity/world.h>
#include <wv/rendering/viewport.h>
#include <wv/camera/view_volume.h>

#include <wv/math/geometry.h>

#include <imgui/imgui.h>

void TrackVehicleSystem::initialize()
{
	
}

void TrackVehicleSystem::shutdown()
{
	for ( TrackJunction* junction : m_trackJunctions )
		WV_FREE( junction );

	for ( TrackLength* trackLength : m_trackLengths )
	{
		trackLength->clear();
		WV_FREE( trackLength );
	}

	m_trackJunctions.clear();
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

	for ( TrackEngineComponent* engine : m_engineComponents.getComponents() )
	{
		double velocity = _ctx.deltaTime * 10.0 * engine->m_throttle;

		double forward = engine->m_invertedDirection ? -1.0 : 1.0;

		engine->m_trackPosition += velocity * forward;

		TrackPosition trackLocation = moveAlongTrack( engine->m_trackIndex, engine->m_trackPosition, engine->m_invertedDirection );
		if ( trackLocation.index < 0 ) 
			continue; // error
		
		engine->m_trackIndex        = trackLocation.index;
		engine->m_trackPosition     = trackLocation.distanceFromStart;
		engine->m_invertedDirection = trackLocation.invertedDirectionOfTravel;

		forward = engine->m_invertedDirection ? -1.0 : 1.0;

		wv::Vector3f frontWheelPos = getTrackWorldPosition( engine->m_trackIndex, engine->m_trackPosition + forward, engine->m_invertedDirection );
		wv::Vector3f backWheelPos  = getTrackWorldPosition( engine->m_trackIndex, engine->m_trackPosition - forward, engine->m_invertedDirection );

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

				TrackLength* trackLength = getTrack( m_buildingTrackPosition.index );
				
				wv::Vector3f startPos = trackLength->getPositionAt( m_buildingTrackPosition.distanceFromStart );
				wv::Vector3f endVector = ( rayhit - startPos ).normalized();
				wv::Vector3f thing = endVector.cross( wv::Vector3f::up() );
				wv::Vector3f thingCentre = ( rayhit + startPos ) / 2;

				wv::Vector3f debugOffset{ 0.0f, 2.0f, 0.0 };
				
				renderer->addDebugLine( 
					debugOffset + startPos, 
					debugOffset + rayhit );

				renderer->addDebugLine( 
					debugOffset + thingCentre, 
					debugOffset + thingCentre + thing );

				wv::Vector3f intersectionPoint{};
				if ( wv::Math::linePlaneIntersection( thingCentre, thing, startPos, trackLength->getDirectionAt( m_buildingTrackPosition.distanceFromStart ), &intersectionPoint) )
				{
					m_currentlyBuildingLength.clear();

					wv::Vector3f dirA = startPos - intersectionPoint;
					wv::Vector3f dirB = rayhit   - intersectionPoint;

					wv::Vector2f a{ dirA.x, dirA.z };
					a.normalize();
					
					wv::Vector2f b{ dirB.x, dirB.z };
					b.normalize();

					float angle = wv::Math::angleBetween( b, a, true );

					m_currentlyBuildingLength.addArcTrack( startPos, intersectionPoint, -wv::Math::degrees( angle ) );

					renderer->addDebugLine( startPos, intersectionPoint );
					renderer->addDebugSphere( intersectionPoint, 1.0f );
				}

				if ( _ctx.inputSystem->getMouseButtonUp( 1 ) )
					endTrackBuild( connectingTrack );
			}
		}
	}
}

static void debugDrawTrackLength( wv::Renderer* _renderer, TrackLength* _trackLength ) {
	double len = _trackLength->length();
	double pos = 0.0;

	if ( len == 0.0f )
		return;

	for ( size_t i = 0; i < (size_t)len + 1; i++ )
	{
		wv::Vector3f lineStart = _trackLength->getPositionAt( pos );
		pos += 1.0;
		wv::Vector3f lineEnd = _trackLength->getPositionAt( pos );

		_renderer->addDebugLine( lineStart, lineEnd );

		if ( _trackLength->nextJunctionIndex != -1 )
			_renderer->addDebugSphere( _trackLength->getEndPosition(), 0.5f );
		else if ( _trackLength->prevJunctionIndex != -1 )
			_renderer->addDebugSphere( _trackLength->getStartPosition(), 0.5f );
	}
}

void TrackVehicleSystem::onDebugRender()
{
	auto renderer = wv::getApp()->getRenderer();

	for ( TrackLength* trackLength : m_trackLengths )
		debugDrawTrackLength( renderer, trackLength );
	
	debugDrawTrackLength( renderer, &m_currentlyBuildingLength );

	if ( ImGui::Begin( "Track Switches" ) )
	{
		for ( size_t i = 0; i < m_trackJunctions.size(); i++ )
		{
			auto junction = m_trackJunctions[ i ];
			ImGui::Text( "%i (Current: %i)", (int)i, junction->currentTrackIndex );
			ImGui::SameLine();
			std::string buttonID = std::format("Switch Track##{}", i);
			if ( ImGui::Button( buttonID.c_str() ) )
			{
				junction->currentTrackIndex++;
				if ( junction->currentTrackIndex >= junction->outIndices.size() )
					junction->currentTrackIndex = 0;
			}
		}
	}
	ImGui::End();

	for ( size_t junctionIndex = 0; junctionIndex < m_trackJunctions.size(); junctionIndex++ )
	{
		TrackJunction* junction = m_trackJunctions[ junctionIndex ];
		TrackLength* inTrack = getTrack( junction->inIndex );
		
		wv::Vector3f offset = wv::Vector3f::up();
		wv::Vector3f pos{};
		wv::Vector3f dir{};

		{
			if ( inTrack->nextJunctionIndex == junctionIndex )
			{
				pos = inTrack->getEndPosition();
				dir = inTrack->getEndDirection();
			}
			else
			{
				pos = inTrack->getStartPosition();
				dir = inTrack->getStartDirection();
			}

			renderer->addDebugLine( pos + offset, pos + dir + offset );
		}

		if ( junction->outIndices.size() == 0 )
			continue;

		{
			size_t outIndex = junction->currentTrackIndex;

			TrackLength* outTrack = getTrack( junction->outIndices[ outIndex ] );
			offset.y += 0.2;

			//wv::Vector3f pos = outTrack->getEndPosition();
			
			if ( outTrack->prevJunctionIndex == junctionIndex )
			{
				double tlen = outTrack->length();
				double travel = 0.0;
				while ( travel < wv::Math::min( 6.0, tlen ) )
				{
					wv::Vector3f p1 = outTrack->getPositionAt( travel );
					travel += 1.0;
					wv::Vector3f p2 = outTrack->getPositionAt( travel );

					renderer->addDebugLine( p1 + offset, p2 + offset );
				}
			}
			else
			{
				double tlen = outTrack->length();
				double travel = 0.0;
				while( travel < wv::Math::min( 6.0, tlen ) )
				{
					wv::Vector3f p1 = outTrack->getPositionAt( tlen - travel );
					travel += 1.0;
					wv::Vector3f p2 = outTrack->getPositionAt( tlen - travel );

					renderer->addDebugLine( p1 + offset, p2 + offset );
				}
			}
		}

		
	}
}

wv::Vector3f TrackVehicleSystem::getTrackWorldPosition( size_t _track, double _position, bool _invertedDirection )
{
	TrackPosition trackLocation = moveAlongTrack( _track, _position, _invertedDirection );

	if ( trackLocation.index == -1 )
		return {};

	TrackLength* track = m_trackLengths[ trackLocation.index ];
	return track->getPositionAt( trackLocation.distanceFromStart );
}

TrackPosition TrackVehicleSystem::getClosestToPoint( const wv::Vector3f& _point ) const
{
	TrackPosition trackPosition{};
	trackPosition.worldPosition = _point;

	float sqrDist = FLT_MAX;

	for ( size_t i = 0; i < m_trackLengths.size(); i++ )
	{
		const TrackLength* trackLength = m_trackLengths[ i ];
	
		const double newTrackPos = trackLength->getClosestTrackPosition( _point );
		const wv::Vector3f newPoint = trackLength->getPositionAt( newTrackPos );

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

TrackPosition TrackVehicleSystem::moveAlongTrack( size_t _track, double _movedPosition, bool _invertedDirection )
{
	if ( _track < 0 )
		return TrackPosition( (int)_track, _movedPosition, _invertedDirection );

	if ( getTrack( _track )->isPositionInsideTrack( _movedPosition ) )
	{
		// still inside of track, no need to check for junctions

		return TrackPosition( _track, _movedPosition, _invertedDirection );
	}

	// not inside track, check for junctions
	
	TrackLength* track = getTrack( _track );

	if ( _movedPosition < 0.0 ) // moving backwards
	{
		if ( track->prevJunctionIndex >= 0 && track->prevJunctionIndex < m_trackJunctions.size() )
		{
			TrackJunction* junction = m_trackJunctions[ track->prevJunctionIndex ];
			int newTrackIndex = junction->getConnectedTrack( _track );
			
			// junction leads to new track
			if ( newTrackIndex != -1 )
			{
				TrackLength* newTrack = m_trackLengths[ newTrackIndex ];

				bool isVJoint = track->prevJunctionIndex == newTrack->prevJunctionIndex;

				if( !isVJoint ) // --> J -->
					return TrackPosition( newTrackIndex, newTrack->length() + _movedPosition, _invertedDirection );
				else // <-- J -->
					return TrackPosition( newTrackIndex, -_movedPosition, !_invertedDirection );
			}
		}
		
		// no junction, or junction did not lead anywhere
		return TrackPosition( _track, 0.0, _invertedDirection );
	}
	else // moving forward
	{
		if ( track->nextJunctionIndex >= 0 && track->nextJunctionIndex < m_trackJunctions.size() )
		{
			TrackJunction* junction = m_trackJunctions[ track->nextJunctionIndex ];
			int newTrackIndex = junction->getConnectedTrack( _track );

			// junction leads to new track
			if ( newTrackIndex != -1 )
			{
				TrackLength* newTrack = m_trackLengths[ newTrackIndex ];

				bool isVJoint = track->nextJunctionIndex == newTrack->nextJunctionIndex;

				if ( !isVJoint ) // --> J -->
					return TrackPosition( newTrackIndex, _movedPosition - track->length(), _invertedDirection );
				else // --> J <--
					return TrackPosition( 
						newTrackIndex, 
						newTrack->length() - ( _movedPosition - track->length() ),
						!_invertedDirection );
			}
		}
		
		// no junction, or junction did not lead anywhere
		return TrackPosition( _track, track->length(), _invertedDirection );
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
	m_currentlyBuildingLength.clear();

	double buildLength = ( _connectTrackPosition.worldPosition - m_buildingTrackPosition.worldPosition ).length();
	if ( buildLength < 1.0 )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "Track too short\n" );
		return;
	}

	size_t newJunctionIndex{};
	
	if ( splitTrackLength( m_buildingTrackPosition.index, m_buildingTrackPosition.distanceFromStart, false, &newJunctionIndex ) )
	{
		size_t newTrackIndex{};
		TrackLength* newTrack = createTrackLength( &newTrackIndex );
		TrackJunction* junction = m_trackJunctions[ newJunctionIndex ];

		junction->outIndices.push_back( newTrackIndex );
		junction->currentTrackIndex++;

		// Create new connecting track

		TrackLength* trackLength = getTrack( m_buildingTrackPosition.index );

		newTrack->prevJunctionIndex = newJunctionIndex;


		// newTrack->addLineTrack( trackLength->getEndPosition(), _connectTrackPosition.worldPosition );
		newTrack->prevJunctionIndex = newJunctionIndex;

		{

			wv::Vector3f buildStartPos = trackLength->getEndPosition();
			wv::Vector3f buildEndPos = _connectTrackPosition.worldPosition;

			wv::Vector3f endVector = ( buildEndPos - buildStartPos ).normalized();
			wv::Vector3f arcNormal = endVector.cross( wv::Vector3f::up() );
			wv::Vector3f arcNormalCentre = ( buildStartPos + buildEndPos ) / 2;

			wv::Vector3f intersectionPoint{};
			if ( wv::Math::linePlaneIntersection( arcNormalCentre, arcNormal, buildStartPos, trackLength->getEndDirection(), &intersectionPoint ) )
			{
				wv::Vector3f dirA = buildStartPos - intersectionPoint;
				wv::Vector3f dirB = buildEndPos - intersectionPoint;

				wv::Vector2f a{ dirA.x, dirA.z };
				a.normalize();

				wv::Vector2f b{ dirB.x, dirB.z };
				b.normalize();

				float angle = wv::Math::angleBetween( b, a, true );

				newTrack->addArcTrack( buildStartPos, intersectionPoint, -wv::Math::degrees( angle ) );
			}

		}

		// Create and connect junction 
		if ( _connectTrackPosition.index != -1 )
		{
			size_t connectingJunctionIndex;

			wv::Vector3f newTrackDir = newTrack->getEndDirection();
			newTrackDir.normalize();
			
			TrackLength* connectingTrack = getTrack( _connectTrackPosition.index );
			wv::Vector3f connectingTrackDir = connectingTrack->getDirectionAt( _connectTrackPosition.distanceFromStart );
			connectingTrackDir.normalize();

			float dot = newTrackDir.dot( connectingTrackDir );
			
			if ( splitTrackLength( _connectTrackPosition.index, _connectTrackPosition.distanceFromStart, dot >= 0.0, &connectingJunctionIndex ) )
			{
				TrackJunction* connectingJunction = m_trackJunctions[ connectingJunctionIndex ];
				connectingJunction->currentTrackIndex++;
				
				connectingJunction->outIndices.push_back( newTrackIndex );
				newTrack->nextJunctionIndex = connectingJunctionIndex;

			}
		}

	}
	else
	{

	}

}

bool TrackVehicleSystem::splitTrackLength( size_t _trackIndex, double _trackPosition, bool _flipJunction, size_t* _outJunctionIndex )
{
	// Split and add upper track segment
	
	TrackLength* lowerTrack = getTrack( _trackIndex );
	TrackLength splitUpperTrack = lowerTrack->splitTrackAt( _trackPosition );

	if ( splitUpperTrack.length() <= 0.0 )
		return false; // no spltting occured, we're at the edge of the track length

	size_t upperTrackIndex;
	TrackLength* upperTrack = createTrackLength( splitUpperTrack, &upperTrackIndex );

	// New junction between the split lengths
	size_t junctionIndex{};
	TrackJunction* junction = createTrackJunction( &junctionIndex );
	if ( _outJunctionIndex )
		*_outJunctionIndex = junctionIndex;

	// make sure any connecting junction on the upper half gets the new index
	if ( lowerTrack->nextJunctionIndex != -1 )
	{
		upperTrack->nextJunctionIndex = lowerTrack->nextJunctionIndex;

		TrackJunction* oldJunction = m_trackJunctions[ lowerTrack->nextJunctionIndex ];
		oldJunction->replaceTrackIndex( _trackIndex, upperTrackIndex );
	}

	if ( !_flipJunction )
	{
		junction->inIndex = _trackIndex;
		junction->outIndices.push_back( upperTrackIndex );

		lowerTrack->nextJunctionIndex = junctionIndex;
		upperTrack->prevJunctionIndex = junctionIndex;
	}
	else
	{
		junction->inIndex = upperTrackIndex;
		junction->outIndices.push_back( _trackIndex );

		lowerTrack->nextJunctionIndex = junctionIndex;
		upperTrack->prevJunctionIndex = junctionIndex;
	}

	for ( TrackEngineComponent* engine : m_engineComponents.getComponents() )
	{
		if ( engine->m_trackIndex != _trackIndex )
			continue;

		if ( engine->m_trackPosition > _trackPosition )
		{
			engine->m_trackIndex = upperTrackIndex;
			engine->m_trackPosition -= lowerTrack->length();
		}
	}

	return true;
}
