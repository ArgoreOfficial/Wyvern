#pragma once

#include <wv/Scene/Component/IComponent.h>

#include <wv/Mesh/MeshResource.h>
#include <BezierTest/BezierTest.h>
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

	BezierTest* Testing = nullptr;
protected:

	void spawnBalls( int _count );
	void spawnBezier(wv::BezierCurve<float> _curvePoints);
	void spawnCubes( int _count );
	void spawnBlock( int _halfX, int _halfY, int _halfZ );

	virtual void onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) override;

	int m_numToSpawn = 10;
	int m_numSpawned = 0;
};
