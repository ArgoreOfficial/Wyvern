#pragma once

#include <wv/scene/entity/entity.h>

#include <wv/mesh/mesh_resource.h>

#include <wv/physics/physics_engine.h>
#include <wv/physics/physics_body_descriptor.h>

#include <wv/runtime.h>
#include <wv/memory/memory.h>

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

struct MeshNode; 
struct MeshResource; 

///////////////////////////////////////////////////////////////////////////////////////

WV_RUNTIME_OBJECT( Rigidbody, Entity )
class Rigidbody : public Entity
{
public:
	Rigidbody() = default;
	Rigidbody( const UUID& _uuid, const std::string& _name );
	Rigidbody( const UUID& _uuid, const std::string& _name, const std::string& _meshPath, IPhysicsBodyDesc* _bodyDesc );
	~Rigidbody();

	virtual FunctionFlags getFunctionFlags() override {
	#ifdef WV_SUPPORT_PHYSICS
		return FunctionFlags::kOnConstruct | FunctionFlags::kOnDestruct 
			| FunctionFlags::kOnEnter | FunctionFlags::kOnExit 
			| FunctionFlags::kOnPhysicsUpdate;
	#else
		return FunctionFlags::kNone;
	#endif
	}

	virtual void onConstruct() override;
	virtual void onDestruct() override;
	virtual void onEnter() override;
	virtual void onExit() override;
	virtual void onPhysicsUpdate( double _dt) override;

	//static Rigidbody* parseInstance( ParseData& _data );

	static void queryProperties( wv::RuntimeProperties* _pOutProps ) {
		_pOutProps->add( "path", &Rigidbody::m_meshPath );
		_pOutProps->add( "kind", &Rigidbody::m_kind );
		_pOutProps->add( "shape", &Rigidbody::m_shape );
	}

protected:
	std::string m_meshPath = "";
	
	int m_kind  = WV_PHYSICS_STATIC;
	int m_shape = WV_PHYSICS_NONE;

	float m_sphereRadius;
	Vector3f m_boxExtents{1,1,1};

#ifdef WV_SUPPORT_PHYSICS
	IPhysicsBodyDesc* m_pPhysicsBodyDesc = nullptr;
	PhysicsBodyID m_physicsBodyHandle = PhysicsBodyID::InvalidID;
#endif
};


}

