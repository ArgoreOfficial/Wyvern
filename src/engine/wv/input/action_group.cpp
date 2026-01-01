#include "action_group.h"

#include <wv/memory/memory.h>
#include <wv/debug/log.h>

#include <wv/input/actions/button_action.h>
#include <wv/input/actions/axis_action.h>

wv::ActionGroup::~ActionGroup()
{
	
}

void wv::ActionGroup::bindTriggerAction( const std::string& _action, const std::string& _device, uint32_t _inputID )
{
	TriggerAction* action = m_triggerActions.getAction( _action );
	
	if ( m_triggerActions.deviceMaps.contains( _device ) )
	{
		// make sure action hasn't already been bound
		for ( auto& map : m_triggerActions.deviceMaps.at( _device ) )
		{
			if ( map.inputID != _inputID || map.action != action )
				continue;

			WV_LOG_WARNING( "Action %s has already been bound to %s:%llu", _action, _device, _inputID );
			return;
		}
	}

	m_triggerActions.deviceMaps[ _device ].emplace_back( _inputID, action );
}

void wv::ActionGroup::bindAxisAction( const std::string& _action, const std::string& _device, AxisActionDirection _direction, uint32_t _inputID )
{
	AxisAction* action = m_axisActions.getAction( _action );

	if ( m_axisActions.deviceMaps.contains( _device ) )
	{
		// make sure action hasn't already been bound
		for ( auto& map : m_axisActions.deviceMaps.at( _device ) )
		{
			if ( map.inputID != _inputID || map.action != action || map.direction != _direction )
				continue;

			WV_LOG_WARNING( "Action %s has already been bound to %s:%llu", _action, _device, _inputID );
			return;
		}
	}

	m_axisActions.deviceMaps[ _device ].emplace_back( _inputID, _direction, action );
}
