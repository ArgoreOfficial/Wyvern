#pragma once

#include <wv/Scene/SceneObject.h>
#include <wv/Reflection/Reflection.h>
#include <wv/Engine/Engine.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

class cDemoWindow : public wv::iSceneObject
{

public:

	 cDemoWindow( const uint64_t& _uuid, const std::string& _name );
	~cDemoWindow();

///////////////////////////////////////////////////////////////////////////////////////
	
	static cDemoWindow* createInstance( void ) { return new cDemoWindow( wv::cEngine::getUniqueUUID(), "cDemoWindow" ); }
	static cDemoWindow* createInstanceYaml( fkyaml::node& _data ) 
	{ 
		wv::UUID    uuid = _data[ "uuid" ].get_value<unsigned int>();
		std::string name = _data[ "name" ].get_value<std::string>();

		return new cDemoWindow( uuid, name ); 
	}

///////////////////////////////////////////////////////////////////////////////////////

protected:

	void spawnBalls( int _count );
	void spawnCubes( int _count );

	void onLoadImpl() override { };
	void onUnloadImpl() override { };
	void onCreateImpl() override { };
	void onDestroyImpl() override { };

	virtual void updateImpl( double _deltaTime ) override;
	virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) override;

	int m_numToSpawn = 10;
	int m_numSpawned = 0;
};

REFLECT_CLASS( cDemoWindow );
