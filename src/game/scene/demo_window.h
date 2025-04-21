#pragma once

#include "demo_window_component.h"

#include <wv/scene/entity/entity.h>
#include <wv/engine.h>

#include <wv/memory/memory.h>

#include <string>

///////////////////////////////////////////////////////////////////////////////////////

WV_RUNTIME_OBJECT( DemoWindow, wv::Entity )
class DemoWindow : public wv::Entity
{

public:

	 DemoWindow( void ) = default;
	 DemoWindow( const uint64_t& _uuid, const std::string& _name ) : wv::Entity{ _uuid, _name } {}
	~DemoWindow() {}

	virtual void onConstruct() override {
		addComponent<DemoWindowComponent>();
	}

};
