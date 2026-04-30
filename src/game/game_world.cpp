#include "game_world.h"

#include <wv/application.h>
#include <wv/serialize.h>
#include <wv/reflection/reflection.h>
#include <wv/rendering/mesh.h>

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

static void to_json( nlohmann::json& _json, const CrateComponent& _comp ) { 
	_json = nlohmann::json{
		// { "cameraSensitivity",  _comp.cameraSensitivity },
	};
}

static void from_json( const nlohmann::json& _json, CrateComponent& _comp ) {
	// _json.at( "cameraSensitivity" ).get_to( _comp.cameraSensitivity );
}

struct Point
{
	wv::Vector2f test1 = {0.0f, 1.0f};
	wv::Ref<wv::MeshAsset> test2 = nullptr;
	std::vector<wv::Ref<wv::MeshAsset>> test3 = { nullptr, nullptr, nullptr };
	
	static inline wv::Reflection reflection{
		wv::reflect( "test1", &Point::test1 ),
		wv::reflect( "test2", &Point::test2 ),
		wv::reflect( "test3", &Point::test3 )
	};
};

void GameWorld::onSceneCreate()
{
	Point p{};
	p.test2 = wv::MeshAsset::get( "meshes/SM_Cube.wvb" );
	p.test3 = {
		wv::MeshAsset::get( "meshes/SM_Cube.wvb" ),
		wv::MeshAsset::get( "meshes/SM_Cube.wvb" ),
		wv::MeshAsset::get( "meshes/SM_Cube.wvb" )
	};

	nlohmann::json j = wv::reflection_of<Point>.serialize( &p );
	printf( "%s\n", j.dump( 2 ).c_str() );

	// Components
	registerComponentType<CrateComponent>( "CrateComponent" );
	
	// Systems
	addSystem<CrateController>();
	
	// Load World
	load( "worlds/test_world.world" );
}

