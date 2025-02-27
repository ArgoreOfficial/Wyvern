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

	bool m_isBuilding3DS = false;
	wv::Fence* m_buildFence = nullptr;

	char m_targetAddressStr[16] = "192.168.0.160";
	int m_targetPort = 4003;

	std::vector<const char*> m_buildPlatforms = {
		"3DS",
		"PSP2"
	};

	std::vector<const char*> m_buildArchs = {
		"arm_3ds",
		"psp2"
	};

	std::vector<const char*> m_buildModes = {
		"Debug",
		"Debug-nomt",
		"Release",
		"Package"
	};

	const char* m_currentBuildPlatform = m_buildPlatforms[ 0 ];
	const char* m_currentBuildArch     = m_buildArchs[ 0 ];
	const char* m_currentBuildMode     = m_buildModes[ 0 ];
	
};
