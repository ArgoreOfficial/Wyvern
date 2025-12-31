#include "action_group.h"

#include <wv/memory/memory.h>
#include <wv/debug/log.h>

#include <wv/input/actions/button_action.h>
#include <wv/input/actions/axis_action.h>

wv::ActionGroup::~ActionGroup()
{
	
}

wv::Vector2<double> wv::ActionGroup::getAxisValue( const std::string& _name ) const
{
	return { 0.0, 0.0 };
}

void wv::ActionGroup::bindTriggerAction( const std::string& _action, const std::string& _device, uint32_t _inputID )
{
	TriggerAction* action = m_triggerActions.getAction( _action );
	
	if ( m_triggerActions.deviceMaps.contains( _device ) )
	{
		// make sure action hasn't already been bound
		for ( auto& map : m_triggerActions.deviceMaps.at( _device ) )
			if ( map.inputID == _inputID && map.action == action )
				return;
	}

	m_triggerActions.deviceMaps[ _device ].emplace_back( _inputID, action );
}
