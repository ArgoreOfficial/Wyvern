#pragma once

#include <wv/entity/ecs.h>
#include <wv/input/action_group.h>

#include "crate_component.h"

class CrateController : public wv::ISystem
{
public:
	virtual void configure( wv::ArchetypeConfig& _config ) override {
		setDebugName( "CrateController" );
		setUpdateMode( wv::UpdateMode_Runtime );

		_config.addComponentType<CrateComponent>();
	}

	virtual void onInitialize() override;
	virtual void onUpdate() override;
	
private:

};
