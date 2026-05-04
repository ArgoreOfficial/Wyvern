#include "game_world.h"

#include <wv/application.h>
#include <wv/serialize.h>
#include <wv/reflection/reflection.h>

#include <wv/components/mesh_component.h>

#include "crate_controller.h"

void GameWorld::onSetupInput( wv::InputSystem* _inputSystem )
{ 
	wv::ActionGroup* playerActionGroup = _inputSystem->createActionGroup( "Player" );

	playerActionGroup->bindTriggerAction( "Shake", "Controller", wv::CONTROLLER_BUTTON_A );
	playerActionGroup->bindTriggerAction( "Shake", "Keyboard", wv::SCANCODE_SPACE );
	
	playerActionGroup->bindTriggerAction( "DebugMouseLock", "Keyboard", wv::SCANCODE_R );

	playerActionGroup->bindAxisAction( "Move", "Controller", wv::AxisActionDirection_All, wv::CONTROLLER_JOYSTICK_LEFT );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AxisActionDirection_North, wv::SCANCODE_W );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AxisActionDirection_South, wv::SCANCODE_S );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AxisActionDirection_East, wv::SCANCODE_D );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AxisActionDirection_West, wv::SCANCODE_A );

	playerActionGroup->bindAxisAction( "Look", "Controller", wv::AxisActionDirection_All, wv::CONTROLLER_JOYSTICK_RIGHT );
	playerActionGroup->bindAxisAction( "Look", "Mouse", wv::AxisActionDirection_All, wv::MouseInput_MotionAxis );

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

