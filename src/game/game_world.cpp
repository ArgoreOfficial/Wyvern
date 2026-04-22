#include "game_world.h"

#include <wv/application.h>

#include <wv/editor/mesh_importer_gltf.h>

#include <wv/entity/ecs.h>
#include <wv/entity/entity.h>

#include <wv/components/mesh_component.h>
#include <wv/components/camera_component.h>
#include <wv/components/rigidbody_component.h>
#include <wv/components/collider_component.h>

#include <wv/serialize.h>

#include "player_move_system.h"

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

static void to_json( nlohmann::json& _json, const PlayerMoveComponent& _comp ) { 
	_json = nlohmann::json{
		{ "cameraSensitivity",  _comp.cameraSensitivity },
		{ "cameraHeight",       _comp.cameraHeight },
		{ "cameraShakeDecay",   _comp.cameraShakeDecay },
		{ "smoothAcceleration", _comp.smoothAcceleration },
		{ "acceleration",       _comp.acceleration },
		{ "moveSpeed",          _comp.moveSpeed },
		{ "damping",            _comp.damping },
		{ "viewBobbing",        _comp.viewBobbing },
		{ "viewBobbingSpeed",   _comp.viewBobbingSpeed },
		{ "resetBobPosition",   _comp.resetBobPosition },
		{ "viewRotting",        _comp.viewRotting },
		{ "viewRotFrequency",   _comp.viewRotFrequency },
		{ "viewRotOffset",      _comp.viewRotOffset }
	};

	if ( _comp.cameraEntity )
		_json[ "cameraEntity" ] = (uint64_t)_comp.cameraEntity->getID();
}

static void from_json( const nlohmann::json& _json, PlayerMoveComponent& _comp ) {
	_json.at( "cameraSensitivity" ).get_to( _comp.cameraSensitivity );
	_json.at( "cameraHeight" ).get_to( _comp.cameraHeight );
	_json.at( "cameraShakeDecay" ).get_to( _comp.cameraShakeDecay );
	_json.at( "smoothAcceleration" ).get_to( _comp.smoothAcceleration );
	_json.at( "acceleration" ).get_to( _comp.acceleration );
	_json.at( "moveSpeed" ).get_to( _comp.moveSpeed );
	_json.at( "damping" ).get_to( _comp.damping );
	_json.at( "viewBobbing" ).get_to( _comp.viewBobbing );
	_json.at( "viewBobbingSpeed" ).get_to( _comp.viewBobbingSpeed );
	_json.at( "resetBobPosition" ).get_to( _comp.resetBobPosition );
	_json.at( "viewRotting" ).get_to( _comp.viewRotting );
	_json.at( "viewRotFrequency" ).get_to( _comp.viewRotFrequency );
	_json.at( "viewRotOffset" ).get_to( _comp.viewRotOffset );

	if ( _json.contains( "cameraEntity" ) )
	{
		uint64_t id = (uint64_t)_json.at( "cameraEntity" );
		_comp.cameraEntity = wv::getApp()->getWorld()->getEntityFromID( id );
	}
}

void GameWorld::onSceneCreate()
{
	// Components
	registerComponentType<PlayerMoveComponent>();
	m_serializer->addComponentFunction<PlayerMoveComponent>();
	
	// Systems
	addSystem<PlayerMoveSystem>();
	
	// Load World
	load( "worlds/test_world.world" );
}
