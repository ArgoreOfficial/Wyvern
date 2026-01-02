#include "keyboard_driver.h"

void wv::IKeyboardDriver::initiailize( InputSystem* _inputSystem )
{
	m_vdID = _inputSystem->requestVirtualDeviceID( this, "Keyboard" );
}

void wv::IKeyboardDriver::shutdown( InputSystem* _inputSystem )
{
	_inputSystem->releaseVirtualDeviceID( m_vdID );
}

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

			handleTriggerAction( _inputSystem, m_vdID, mapping.action, m_scancodeStates[ mapping.inputID ] );
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

			handleValueAction( _inputSystem, m_vdID, mapping.action, m_scancodeStates[ mapping.inputID ] ? 1.0f : 0.0f );
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

			handleAxisAction( _inputSystem, m_vdID, mapping.action, mapping.direction, m_scancodeStates[ mapping.inputID ] ? 1.0f : -1.0f, true );
		}
	}
}
