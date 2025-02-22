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
	RigidBodyComponent( IPhysicsBodyDesc* _bodyDesc ) 
		: IComponent( "RigidBodyComponent" )
	#ifdef WV_SUPPORT_PHYSICS
		, m_pPhysicsBodyDesc{ _bodyDesc }
	#endif
	{ }

	virtual FunctionFlags getFunctionFlags() override {
	#ifdef WV_SUPPORT_PHYSICS
		return FunctionFlags::kOnEnter | FunctionFlags::kOnExit | FunctionFlags::kOnPhysicsUpdate;
	#else
		return FunctionFlags::kNone;
	#endif
	}

	virtual void onEnter( void ) override;
	virtual void onExit( void ) override;

	virtual void onPhysicsUpdate( double _deltaTime ) override;

protected:
#ifdef WV_SUPPORT_PHYSICS
	IPhysicsBodyDesc* m_pPhysicsBodyDesc = nullptr;
	PhysicsBodyID m_physicsBodyHandle = PhysicsBodyID::InvalidID;
#endif
};

}