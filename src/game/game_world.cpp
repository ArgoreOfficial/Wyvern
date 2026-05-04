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

	playerActionGroup->bindAxisAction( "Move", "Controller", wv::AXIS_DIRECTION_ALL, wv::CONTROLLER_JOYSTICK_LEFT );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_NORTH, wv::SCANCODE_W );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_SOUTH, wv::SCANCODE_S );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_EAST, wv::SCANCODE_D );
	playerActionGroup->bindAxisAction( "Move", "Keyboard", wv::AXIS_DIRECTION_WEST, wv::SCANCODE_A );

	playerActionGroup->bindAxisAction( "Look", "Controller", wv::AXIS_DIRECTION_ALL, wv::CONTROLLER_JOYSTICK_RIGHT );
	playerActionGroup->bindAxisAction( "Look", "Mouse", wv::AXIS_DIRECTION_ALL, wv::MOUSE_MOTION_AXIS );

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

