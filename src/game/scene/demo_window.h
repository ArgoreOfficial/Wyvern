#pragma once

#include "demo_window_component.h"

#include <wv/scene/entity/entity.h>
#include <wv/reflection/reflection.h>
#include <wv/engine.h>

#include <wv/memory/memory.h>

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
		DemoWindow* dw = WV_NEW( DemoWindow, wv::Engine::getUniqueUUID(), "DemoWindow" );
		dw->addComponent<DemoWindowComponent>();
		return dw;
	}

	static DemoWindow* parseInstance( wv::ParseData& _data ) { 
		wv::Json& json = _data.json;
		wv::UUID    uuid = json[ "uuid" ].int_value();
		std::string name = json[ "name" ].string_value();

		DemoWindow* dw = WV_NEW( DemoWindow, uuid, name );
		dw->addComponent<DemoWindowComponent>();
		return dw;
	}

};
