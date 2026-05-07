#include "game_world.h"

#include <wv/application.h>
#include <wv/reflection.h>
#include <wv/reflection.h>

#include <wv/components/mesh_component.h>

#include "crate_controller.h"

void GameWorld::onSetupInput( wv::InputSystem* _inputSystem )
{ 
	wv::ActionGroup* playerActionGroup = _inputSystem->createActionGroup( "Player" );

	playerActionGroup->bindValueAction( "ChargeJump", "Controller", wv::ControllerInput_ButtonA );
	playerActionGroup->bindValueAction( "ChargeJump", "Keyboard", wv::Scancode_Space );
	
	playerActionGroup->bindAxisAction( "Aim", "Controller", wv::AxisActionDirection_All, wv::ControllerInput_JoystickLeft );
	playerActionGroup->bindAxisAction( "Aim", "Keyboard", wv::AxisActionDirection_North, wv::Scancode_W );
	playerActionGroup->bindAxisAction( "Aim", "Keyboard", wv::AxisActionDirection_South, wv::Scancode_S );
	playerActionGroup->bindAxisAction( "Aim", "Keyboard", wv::AxisActionDirection_East, wv::Scancode_D );
	playerActionGroup->bindAxisAction( "Aim", "Keyboard", wv::AxisActionDirection_West, wv::Scancode_A );

	playerActionGroup->bindAxisAction( "Look", "Controller", wv::AxisActionDirection_All, wv::ControllerInput_JoystickRight );
	playerActionGroup->bindAxisAction( "Look", "Mouse", wv::AxisActionDirection_All, wv::MouseInput_MotionAxis );
	
	playerActionGroup->bindValueAction( "Grow", "Controller", wv::Scancode_LeftShift );
	playerActionGroup->bindValueAction( "Grow", "Keyboard", wv::Scancode_LeftShift );

	playerActionGroup->enable();
}

void GameWorld::onSceneCreate()
{
	// Components
	registerComponentType<CrateComponent>( "CrateComponent" );
	
	// Systems
	addSystem<CrateController>();
	
	// Load World
	load( "worlds/test_world.world" );
}

