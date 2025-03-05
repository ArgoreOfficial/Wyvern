#pragma once

#include <wv/scene/component/icomponent.h>

#include <wv/mesh/mesh_resource.h>
#include <wv/physics/physics_types.h>
#include <wv/event/events.h>

struct IPhysicsBodyDesc;

class DemoWindowComponent : public wv::IComponent
{
public:
	DemoWindowComponent() :
		IComponent( "DemoWindowComponent" )
	{ }

	virtual FunctionFlags getFunctionFlags() override {
		return kOnDraw | kOnEnter | kOnUpdate;
	}

	virtual void onEnter( void ) override;
	virtual void onUpdate( double _dt ) override;

protected:

	void spawnBalls();
	void spawnCubes();
	void spawnBlock( int _halfX, int _halfY, int _halfZ );

	void drawDemoWindow();

	virtual void onDraw( wv::IDeviceContext* _context, wv::IGraphicsDevice* _device ) override;

	int m_numToSpawn = 10;
	int m_numSpawned = 0;

	bool m_isMovingCube = false;

	wv::Entity* m_mouseMarker = nullptr;
};
