#pragma once

#include <wv/entity/entity_component.h>
#include <wv/entity/world_system.h>
#include <wv/entity/entity_component_container.h>
#include <wv/memory/memory.h>

#include "track.h"

#include <utility>

class TrackVehicleComponent : public wv::IEntityComponent
{
	friend class TrackVehicleSystem;

	WV_REFLECT_TYPE( TrackVehicleComponent, wv::IEntityComponent )
public:

	size_t m_trackIndex = 0;
	double m_trackPosition = 0.0;
	bool m_invertedDirection = false;

protected:
};

class TrackEngineComponent : public TrackVehicleComponent
{
	friend class TrackVehicleSystem;

	WV_REFLECT_TYPE( TrackEngineComponent, TrackVehicleComponent )
public:

	void setThrottle( double _throttle ) {
		m_throttle = wv::Math::clamp( _throttle, -1.0, 1.0 );
	}

protected:
	double m_throttle = 0.0;
};

struct TrackPosition
{
	TrackPosition() = default;
	
	TrackPosition( int _index, double _distance, bool _invertedTravel = false, wv::Vector3f _worldPos = {} ) :
		index{ _index },
		distanceFromStart{ _distance },
		invertedDirectionOfTravel{ _invertedTravel },
		worldPosition{ _worldPos }
	{}

	int index = -1;
	double distanceFromStart = 0.0;
	bool invertedDirectionOfTravel = false;

	wv::Vector3f worldPosition{};

};

class TrackVehicleSystem : public wv::IWorldSystem
{
	WV_REFLECT_TYPE( TrackVehicleSystem, wv::IWorldSystem )
public:
	TrackVehicleSystem() = default;

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

protected:
	virtual void initialize() override;
	virtual void shutdown() override;

	virtual void registerComponent( wv::Entity* _entity, wv::IEntityComponent* _component ) override;
	virtual void unregisterComponent( wv::Entity* _entity, wv::IEntityComponent* _component ) override;

	virtual void update( wv::WorldUpdateContext& _ctx ) override;

	virtual void onDebugRender() override;

	wv::Vector3f getTrackWorldPosition( size_t _index, double _trackPosition, bool _invertedDirection = false );
	TrackLength* getTrack( size_t _index ) { return m_trackLengths[ _index ]; }

	TrackPosition getClosestToPoint( const wv::Vector3f& _point ) const;
	TrackPosition moveAlongTrack( size_t _track, double _movedPosition, bool _invertedDirection );

	void beginTrackBuild( TrackPosition _trackPosition );
	void endTrackBuild( TrackPosition _connectTrackPosition );

	bool splitTrackLength( size_t _trackIndex, double _trackPosition, bool _flipJunction, size_t* _outJunctionIndex );

	wv::EntityComponentContainer<TrackEngineComponent> m_engineComponents;

	std::vector<TrackVehicleComponent*> m_vehicleComponents;

	std::vector<TrackLength*>   m_trackLengths{};
	std::vector<TrackJunction*> m_trackJunctions{};

	bool m_isBuildingTrack = false;
	TrackPosition m_buildingTrackPosition;

};
