#pragma once

#include <wv/scene/component/icomponent.h>

#include <wv/mesh/mesh_resource.h>
#include <wv/physics/physics_types.h>

struct IPhysicsBodyDesc;

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

	void drawDemoWindow();
	void drawBuildWindow();

	void buildPlatform();
	void buildAndRun();

	virtual void onDraw( wv::IDeviceContext* _context, wv::IGraphicsDevice* _device ) override;

	int m_numToSpawn = 10;
	int m_numSpawned = 0;

	bool mIsBuilding3DS = false;
	wv::Fence* mBuildFence = nullptr;

	int mTargetAddress[ 4 ] = { 0,0,0,0 };

	std::vector<const char*> mBuildPlatforms = {
		"3DS",
		"PSP2"
	};

	std::vector<const char*> mBuildArchs = {
		"arm_3ds",
		"psp2"
	};

	std::vector<const char*> mBuildModes = {
		"Debug",
		"Debug-nomt",
		"Release",
		"Package"
	};

	const char* mCurrentBuildPlatform = mBuildPlatforms[ 0 ];
	const char* mCurrentBuildMode     = mBuildModes[ 0 ];
	
};
