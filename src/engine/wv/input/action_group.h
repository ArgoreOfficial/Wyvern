#pragma once

#include <wv/debug/error.h>
#include <wv/input/input_enums.h>
#include <wv/input/action.h>
#include <wv/math/vector2.h>
#include <wv/memory/memory.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace wv {

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

	ActionID getActionID( const std::string& _name ) const {
		if ( !nameMap.contains( _name ) ) return 0;
		return nameMap.at( _name )->actionID;
	}

	std::vector<MapTy> getActionsByDevice( const std::string& _deviceName ) const {
		if ( !deviceMaps.contains( _deviceName ) ) return {};
		return deviceMaps.at( _deviceName );
	}

};

class ActionGroup
{
public:
	ActionGroup( const std::string& _name ) 
		: m_name{ _name } 
	{ }
	
	~ActionGroup();

	void enable()  { m_isEnabled = true; }
	void disable() { m_isEnabled = false; }
	
	void bindTriggerAction( const std::string& _action, const std::string& _device, uint32_t _inputID );
	void bindValueAction( const std::string& _action, const std::string& _device, uint32_t _inputID );
	void bindAxisAction( const std::string& _action, const std::string& _device, AxisActionDirection _direction, uint32_t _inputID );

	bool        isEnabled() const { return m_isEnabled; }
	std::string getName()   const { return m_name; }

	bool getTriggerState( int _playerID, const std::string& _name ) const {
		if ( !m_triggerActions.nameMap.contains( _name ) ) return false;
		return m_triggerActions.nameMap.at( _name )->getValue( _playerID );
	}

	wv::Vector2f getAxisValue( int _playerID, const std::string& _name ) const {
		if ( !m_axisActions.nameMap.contains( _name ) ) return { 0.0f, 0.0f };
		return m_axisActions.nameMap.at( _name )->getValue( _playerID );
	}
	
	float getValue( int _playerID, const std::string& _name ) const {
		if ( !m_valueActions.nameMap.contains( _name ) ) return 0.0f;
		return m_valueActions.nameMap.at( _name )->getValue( _playerID );
	}

	inline ActionID getTriggerActionID( const std::string& _name ) const { 
		ActionID id = m_triggerActions.getActionID( _name ); 
		if ( id == 0 )
		{
			WV_LOG_ERROR( "There is no trigger action named '%s' in group '%s'\n", _name.c_str(), m_name.c_str() );
		}

		return id;
	}

	inline ActionID getValueActionID( const std::string& _name ) const { 
		ActionID id = m_valueActions.getActionID( _name );
		if ( id == 0 )
		{
			WV_LOG_ERROR( "There is no value action named '%s' in group '%s'\n", _name.c_str(), m_name.c_str() );
		}

		return id;
	}

	inline ActionID getAxisActionID( const std::string& _name ) const { 
		ActionID id = m_axisActions.getActionID( _name );
		if ( id == 0 )
		{
			WV_LOG_ERROR( "There is no axis action named '%s' in group '%s'\n", _name.c_str(), m_name.c_str() );
		}

		return id;
	}

	std::vector<TriggerActionMapping> getTriggerActionsByDevice( const std::string& _deviceName ) const {
		return m_triggerActions.getActionsByDevice( _deviceName );
	}

	std::vector<ValueActionMapping> getValueActionsByDevice( const std::string& _deviceName ) const {
		return m_valueActions.getActionsByDevice( _deviceName );
	}

	std::vector<AxisActionMapping> getAxisActionsByDevice( const std::string& _deviceName ) const {
		return m_axisActions.getActionsByDevice( _deviceName );
	}

private:
	std::string m_name;

	bool m_isEnabled = false;

	ActionContainer<TriggerAction, TriggerActionMapping> m_triggerActions;
	ActionContainer<ValueAction, ValueActionMapping> m_valueActions;
	ActionContainer<AxisAction, AxisActionMapping> m_axisActions;

};

}