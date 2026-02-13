#pragma once

#include <wv/entity/entity_component.h>
#include <wv/entity/world_system.h>
#include <wv/entity/entity_component_container.h>

#include "track.h"

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
	double m_throttle = 1.0;
};

class TrackVehicleSystem : public wv::IWorldSystem
{
	WV_REFLECT_TYPE( TrackVehicleSystem, wv::IWorldSystem )
public:
	TrackVehicleSystem() = default;

	void addTrackLength( const TrackLength& _trackLength ) {
		m_trackLengths.push_back( _trackLength );
	}

protected:
	virtual void initialize() override;
	virtual void shutdown() override;

	virtual void registerComponent( wv::Entity* _entity, wv::IEntityComponent* _component ) override;
	virtual void unregisterComponent( wv::Entity* _entity, wv::IEntityComponent* _component ) override;

	void update( wv::WorldUpdateContext& _ctx ) override;

	wv::Vector3f getTrackWorldPosition( size_t _index, double _trackPosition );
	TrackLength& getTrack( size_t _index ) { return m_trackLengths[ _index ]; }

	wv::EntityComponentContainer<TrackEngineComponent> m_engineComponents;

	std::vector<TrackVehicleComponent*> m_vehicleComponents;
	std::vector<TrackLength> m_trackLengths{};
};
