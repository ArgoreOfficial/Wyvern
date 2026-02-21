#pragma once

#include <wv/entity/entity_component.h>
#include <wv/entity/world_system.h>
#include <wv/entity/entity_component_container.h>

#include "track.h"

#include <utility>

class TrackVehicleComponent : public wv::IEntityComponent
{
	friend class TrackVehicleSystem;

	WV_REFLECT_TYPE( TrackVehicleComponent, wv::IEntityComponent )
public:

	size_t m_trackIndex = 0;
	double m_trackPosition = 0.0;
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
	int index = -1;
	double distanceFromStart = 0.0;
	wv::Vector3f worldPosition{};
};

class TrackVehicleSystem : public wv::IWorldSystem
{
	WV_REFLECT_TYPE( TrackVehicleSystem, wv::IWorldSystem )
public:
	TrackVehicleSystem() = default;

	void addTrackLength( const TrackLength& _trackLength ) {
		m_trackLengths.push_back( _trackLength );
	}

	void addTrackJunction( const TrackJunction& _trackJunction ) {
		m_trackJunctions.push_back( _trackJunction );
	}

	const std::vector<TrackLength>& getTrackLengths() const { return m_trackLengths; }

protected:
	virtual void initialize() override;
	virtual void shutdown() override;

	virtual void registerComponent( wv::Entity* _entity, wv::IEntityComponent* _component ) override;
	virtual void unregisterComponent( wv::Entity* _entity, wv::IEntityComponent* _component ) override;

	virtual void update( wv::WorldUpdateContext& _ctx ) override;

	virtual void onDebugRender() override;

	wv::Vector3f getTrackWorldPosition( size_t _index, double _trackPosition );
	TrackLength& getTrack( size_t _index ) { return m_trackLengths[ _index ]; }

	TrackPosition getClosestToPoint( const wv::Vector3f& _point ) const;

	std::pair<int, double> moveAlongTrack( size_t _track, double _movedPosition );

	wv::EntityComponentContainer<TrackEngineComponent> m_engineComponents;

	std::vector<TrackVehicleComponent*> m_vehicleComponents;

	std::vector<TrackLength>   m_trackLengths{};
	std::vector<TrackJunction> m_trackJunctions{};
};
