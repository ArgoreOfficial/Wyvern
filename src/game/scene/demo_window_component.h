#pragma once

#include <wv/Scene/Component/IComponent.h>

#include <wv/Mesh/MeshResource.h>
#include <wv/Physics/PhysicsTypes.h>

struct iPhysicsBodyDesc;

class DemoWindowComponent : public wv::IComponent
{
public:
	DemoWindowComponent() :
		IComponent( "DemoWindowComponent" )
	{ }

	virtual FunctionFlags getFunctionFlags() override {
		return FunctionFlags::kOnDraw;
	}

protected:

	void spawnBalls();
	void spawnCubes();
	void spawnBlock( int _halfX, int _halfY, int _halfZ );

	virtual void onDraw( wv::iDeviceContext* _context, wv::IGraphicsDevice* _device ) override;

	int m_numToSpawn = 10;
	int m_numSpawned = 0;
};
