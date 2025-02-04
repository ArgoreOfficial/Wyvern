#pragma once

#include <wv/Scene/SceneObject.h>
#include <wv/Reflection/Reflection.h>
#include <wv/Engine/Engine.h>

#include <wv/Memory/Memory.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

class DemoWindow : public wv::IEntity
{

public:

	 DemoWindow( const uint64_t& _uuid, const std::string& _name );
	~DemoWindow();

///////////////////////////////////////////////////////////////////////////////////////
	
	static DemoWindow* createInstance( void ) { return WV_NEW( DemoWindow, wv::cEngine::getUniqueUUID(), "DemoWindow" ); }
	static DemoWindow* parseInstance( wv::sParseData& _data ) 
	{ 
		wv::Json& json = _data.json;
		wv::UUID    uuid = json[ "uuid" ].int_value();
		std::string name = json[ "name" ].string_value();

		return WV_NEW( DemoWindow, uuid, name );
	}

///////////////////////////////////////////////////////////////////////////////////////

protected:

	void spawnBalls( int _count );
	void spawnCubes( int _count );
	void spawnBlock( int _halfX, int _halfY, int _halfZ );

	void onConstructImpl() override { };
	void onDeconstructImpl() override { };
	void onEnterImpl() override { };
	void onExitImpl() override { };

	virtual void onUpdate( double _deltaTime ) override;
	virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) override;

	int m_numToSpawn = 10;
	int m_numSpawned = 0;
};
