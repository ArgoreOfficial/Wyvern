#pragma once

#include "IComponent.h"

#include <wv/Mesh/MeshResource.h>
#include <wv/Physics/PhysicsTypes.h>

namespace wv
{

struct iPhysicsBodyDesc;

class RigidBodyComponent : public IComponent
{
public:
	RigidBodyComponent( iPhysicsBodyDesc* _bodyDesc ) :
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

	iPhysicsBodyDesc* m_pPhysicsBodyDesc = nullptr;
	PhysicsBodyID m_physicsBodyHandle = PhysicsBodyID::InvalidID;
};

}