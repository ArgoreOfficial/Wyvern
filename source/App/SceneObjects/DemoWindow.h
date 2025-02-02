#pragma once

#include <wv/Scene/SceneObject.h>
#include <wv/Reflection/Reflection.h>
#include <wv/Engine/Engine.h>

#include <wv/Memory/Memory.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

class cDemoWindow : public wv::iSceneObject
{

public:

	 cDemoWindow( const uint64_t& _uuid, const std::string& _name );
	~cDemoWindow();

///////////////////////////////////////////////////////////////////////////////////////
	
	static cDemoWindow* createInstance( void ) { return WV_NEW( cDemoWindow, wv::cEngine::getUniqueUUID(), "cDemoWindow" ); }
	static cDemoWindow* parseInstance( wv::sParseData& _data ) 
	{ 
		wv::Json& json = _data.json;
		wv::UUID    uuid = json[ "uuid" ].int_value();
		std::string name = json[ "name" ].string_value();

		return WV_NEW( cDemoWindow, uuid, name );
	}

///////////////////////////////////////////////////////////////////////////////////////

protected:

	void spawnBalls( int _count );
	void spawnCubes( int _count );
	void spawnBlock( int _halfX, int _halfY, int _halfZ );

	void onLoadImpl() override { };
	void onUnloadImpl() override { };
	void onCreateImpl() override { };
	void onDestroyImpl() override { };

	virtual void onUpdate( double _deltaTime ) override;
	virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) override;

	int m_numToSpawn = 10;
	int m_numSpawned = 0;
};
