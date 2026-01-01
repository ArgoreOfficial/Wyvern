#include "keyboard_driver.h"

#include <wv/input/input_system.h>

void wv::IKeyboardDriver::sendTriggerEvents( InputSystem* _inputSystem )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getTriggerActionsByDevice( "Keyboard" ) )
		{
			if ( !scancodeStateChanged( mapping.inputID ) ) continue;
			if ( mapping.inputID >= SCANCODE_MAX ) continue;

			handleTriggerAction( _inputSystem, mapping.action, m_scancodeStates[ mapping.inputID ] );
		}
	}
}

void wv::IKeyboardDriver::sendValueEvents( InputSystem* _inputSystem )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getValueActionsByDevice( "Keyboard" ) )
		{
			if ( !scancodeStateChanged( mapping.inputID ) ) continue;
			if ( mapping.inputID >= SCANCODE_MAX ) continue;

			handleValueAction( _inputSystem, mapping.action, m_scancodeStates[ mapping.inputID ] ? 1.0f : 0.0f );
		}
	}
}

void wv::IKeyboardDriver::sendAxisEvents( InputSystem* _inputSystem )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getAxisActionsByDevice( "Keyboard" ) )
		{
			if ( !scancodeStateChanged( mapping.inputID ) ) continue;
			if ( mapping.inputID >= SCANCODE_MAX ) continue;

			handleAxisAction( _inputSystem, mapping.action, mapping.direction, m_scancodeStates[ mapping.inputID ] ? 1.0f : -1.0f, true );
		}
	}
}
