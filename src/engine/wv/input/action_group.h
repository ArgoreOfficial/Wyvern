#pragma once

#include <wv/debug/error.h>
#include <wv/input/input_enums.h>
#include <wv/math/vector2.h>
#include <wv/memory/memory.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace wv {

class IAction;
class ButtonAction;

typedef uint32_t ActionID;

enum ActionType
{
	ACTION_TYPE_TRIGGER,
	ACTION_TYPE_VALUE,
	ACTION_TYPE_AXIS
};

struct TriggerAction
{
	TriggerAction( const std::string& _name ) : name{ _name } { }

	const std::string name;
	const ActionID actionID = wv::Math::randomU32();
	bool currentState = false;
};

struct TriggerActionMapping
{
	uint32_t inputID;
	TriggerAction* action;
};

struct AxisAction
{
	AxisAction( const std::string& _name ) : name{ _name } { }

	const std::string name;
	const ActionID actionID = wv::Math::randomU32();

	wv::Vector2f value;
};

enum AxisActionDirection
{
	AXIS_DIRECTION_NORTH,
	AXIS_DIRECTION_SOUTH,
	AXIS_DIRECTION_EAST,
	AXIS_DIRECTION_WEST,
	
	AXIS_DIRECTION_VERTICAL,
	AXIS_DIRECTION_HORIZONTAL,

	AXIS_DIRECTION_ALL // Used with joysticks
};

struct AxisActionMapping
{
	uint32_t inputID = 0;
	AxisActionDirection direction = AXIS_DIRECTION_ALL;
	AxisAction* action = nullptr;
};

template<typename Ty, typename MapTy>
struct ActionContainer
{
	std::vector<Ty*> actions;
	std::unordered_map<std::string, Ty*> nameMap;
	std::unordered_map<std::string, std::vector<MapTy>> deviceMaps;

	~ActionContainer() {
		for ( Ty* action : actions )
			WV_FREE( action );
	}

	Ty* getAction( const std::string& _actionName ) {
		if ( !nameMap.contains( _actionName ) )
		{
			Ty* action = WV_NEW( Ty, _actionName );
			nameMap.emplace( _actionName, action );
			actions.push_back( action );
			return action;
		}
		else
			return nameMap.at( _actionName );
	}
};

class ActionGroup
{
public:
	ActionGroup( const std::string& _name ) 
		: m_name{ _name } 
	{ }
	
	~ActionGroup();

	void enable() {
		WV_ASSERT( m_isEnabled == true );
		m_isEnabled = true;
	}

	void disable() {
		WV_ASSERT( m_isEnabled == false );
		m_isEnabled = false;
	}

	bool        isEnabled() const { return m_isEnabled; }
	std::string getName()   const { return m_name; }

	void bindTriggerAction( const std::string& _action, const std::string& _device, uint32_t _inputID );
	void bindAxisAction( const std::string& _action, const std::string& _device, AxisActionDirection _direction, uint32_t _inputID );

	wv::Vector2f getAxisValue( const std::string& _name ) const {
		if ( !m_axisActions.nameMap.contains( _name ) ) return { 0.0f, 0.0f };
		return m_axisActions.nameMap.at( _name )->value;
	}

	ActionID getTriggerActionID( const std::string& _name ) {
		if ( !m_triggerActions.nameMap.contains( _name ) ) return 0;
		return m_triggerActions.nameMap.at( _name )->actionID;
	}

	ActionID getAxisActionID( const std::string& _name ) {
		if ( !m_axisActions.nameMap.contains( _name ) ) return 0;
		return m_axisActions.nameMap.at( _name )->actionID;
	}

	std::vector<TriggerActionMapping> getTriggerActionsByDevice( const std::string& _deviceName ) {
		if ( !m_triggerActions.deviceMaps.contains( _deviceName ) ) return {};
		return m_triggerActions.deviceMaps.at( _deviceName );
	}

	std::vector<AxisActionMapping> getAxisActionsByDevice( const std::string& _deviceName ) {
		if ( !m_axisActions.deviceMaps.contains( _deviceName ) ) return {};
		return m_axisActions.deviceMaps.at( _deviceName );
	}

private:
	std::string m_name;

	bool m_isEnabled = false;

	ActionContainer<TriggerAction, TriggerActionMapping> m_triggerActions;
	ActionContainer<AxisAction, AxisActionMapping> m_axisActions;

};

}