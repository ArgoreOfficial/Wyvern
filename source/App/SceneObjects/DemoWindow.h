#pragma once

#include "DemoWindowComponent.h"

#include <wv/Scene/Entity/Entity.h>
#include <wv/Reflection/Reflection.h>
#include <wv/Engine/Engine.h>

#include <wv/Memory/Memory.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

class DemoWindow : public wv::Entity
{

public:

	DemoWindow( const uint64_t& _uuid, const std::string& _name ) : 
		wv::Entity{ _uuid, _name } 
	{}
	~DemoWindow() {}

///////////////////////////////////////////////////////////////////////////////////////
	
	static DemoWindow* createInstance( void ) { 
		DemoWindow* dw = WV_NEW( DemoWindow, wv::cEngine::getUniqueUUID(), "DemoWindow" );
		dw->addComponent<DemoWindowComponent>();
		return dw;
	}

	static DemoWindow* parseInstance( wv::sParseData& _data ) { 
		wv::Json& json = _data.json;
		wv::UUID    uuid = json[ "uuid" ].int_value();
		std::string name = json[ "name" ].string_value();

		DemoWindow* dw = WV_NEW( DemoWindow, uuid, name );
		dw->addComponent<DemoWindowComponent>();
		return dw;
	}

};
