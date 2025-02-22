#pragma once

#include "icomponent.h"

#include <wv/mesh/mesh_resource.h>
#include <wv/physics/physics_types.h>

namespace wv
{

struct IPhysicsBodyDesc;

class RigidBodyComponent : public IComponent
{
public:
	RigidBodyComponent( IPhysicsBodyDesc* _bodyDesc ) :
		IComponent( "RigidBodyComponent" ),
		m_pPhysicsBodyDesc{ _bodyDesc }
	{ }

	virtual FunctionFlags getFunctionFlags() override {
		return FunctionFlags::kOnEnter | FunctionFlags::kOnExit | FunctionFlags::kOnPhysicsUpdate;
	}

	virtual void onEnter( void ) override;
	virtual void onExit( void ) override;

	virtual void onPhysicsUpdate( double _deltaTime ) override;

protected:

	IPhysicsBodyDesc* m_pPhysicsBodyDesc = nullptr;
	PhysicsBodyID m_physicsBodyHandle = PhysicsBodyID::InvalidID;
};

}