#pragma once

#include <wv/Scene/SceneObject.h>
#include <wv/Reflection/Reflection.h>
#include <wv/Engine/Engine.h>

#include <string>
#include <vector>


///////////////////////////////////////////////////////////////////////////////////////

struct sTentacleSetting
{
	float frequency;
	float phase;
	float amplitude;
	float shift;

	void drawInputs( int _i );
	float getValue( float _t );
};

///////////////////////////////////////////////////////////////////////////////////////

class cTentacleSettingWindowObject : public wv::iSceneObject
{

public:

	 cTentacleSettingWindowObject( const uint64_t& _uuid, const std::string& _name );
	~cTentacleSettingWindowObject();

///////////////////////////////////////////////////////////////////////////////////////
	
	static cTentacleSettingWindowObject* createInstance( void ) { return nullptr; }
	static cTentacleSettingWindowObject* createInstanceJson( nlohmann::json& _json ) 
	{ 
		wv::UUID    uuid = _json.value( "uuid", wv::cEngine::getUniqueUUID() );
		std::string name = _json.value( "name", "cTentacleSettingWindowObject" );

		return new cTentacleSettingWindowObject( uuid, name ); 
	}

///////////////////////////////////////////////////////////////////////////////////////

	static inline sTentacleSetting settings[3] = {
		{ 0.5f,  0.0f, 1.0f,  0.0f },
		{ 0.0f,  0.0f, 0.0f, -5.1f },
		{ 1.5f, 1.57f, 1.0f, 30.0f }
	};

protected:

	void onLoadImpl() override { };
	void onUnloadImpl() override { };
	void onCreateImpl() override { };
	void onDestroyImpl() override { };

	virtual void updateImpl( double _deltaTime ) override;
	virtual void drawImpl  ( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device ) override;

};

REFLECT_CLASS( cTentacleSettingWindowObject );
