#pragma once

#include <wv/entity/ecs.h>
#include <wv/input/action_group.h>

#include <wv/components/rigidbody_component.h>
#include <wv/components/mesh_component.h>

#include "crate_component.h"

class CrateController : public wv::ISystem
{
public:
	virtual void configure( wv::ArchetypeConfig& _config ) override {
		setDebugName( "CrateController" );
		setUpdateMode( wv::UpdateMode_Runtime );

		_config.addComponentType<CrateComponent>();
		_config.addComponentType<wv::RigidBodyComponent>();
	}

	virtual void onInitialize() override;
	virtual void onUpdate() override;
	
private:

	wv::ActionGroup* m_playerActionGroup = nullptr;
	wv::ActionID m_moveAction;
	wv::ActionID m_jumpAction;
	wv::ActionID m_growAction;
};
