#include "mouse_driver.h"

#include <wv/application.h>

void wv::IMouseDriver::initialize( InputSystem* _inputSystem )
{

}

void wv::IMouseDriver::shutdown( InputSystem* _inputSystem )
{

}

void wv::IMouseDriver::sendTriggerEvents( InputSystem* _inputSystem )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getTriggerActionsByDevice( "Mouse" ) )
		{
			if ( mapping.inputID >= MOUSE_BUTTON_LEFT && mapping.inputID <= MOUSE_BUTTON_X2 && hasStateChanged( (MouseInputs)mapping.inputID ) )
				handleTriggerAction( _inputSystem, m_vdID, mapping.action, m_state.buttonStates[ mapping.inputID ] );
			
			if ( mapping.inputID == MOUSE_SCROLL_DELTA && hasStateChanged( (MouseInputs)mapping.inputID ) )
				handleTriggerAction( _inputSystem, m_vdID, mapping.action, m_state.scrollDelta != 0 );
		}
	}
}

void wv::IMouseDriver::sendValueEvents( InputSystem* _inputSystem )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getValueActionsByDevice( "Mouse" ) )
		{
			if ( mapping.inputID >= MOUSE_BUTTON_LEFT && mapping.inputID <= MOUSE_BUTTON_X2 && hasStateChanged( (MouseInputs)mapping.inputID ) )
				handleValueAction( _inputSystem, m_vdID, mapping.action, m_state.buttonStates[ mapping.inputID ] ? 1.0f : 0.0f );
			
			if ( mapping.inputID == MOUSE_SCROLL_DELTA && hasStateChanged( MOUSE_SCROLL_DELTA ) )
				handleValueAction( _inputSystem, m_vdID, mapping.action, m_state.scrollDelta );
		}
	}
}

void wv::IMouseDriver::sendAxisEvents( InputSystem* _inputSystem )
{
	for ( ActionGroup* group : _inputSystem->getActionGroups() )
	{
		if ( !group->isEnabled() )
			continue;

		for ( auto& mapping : group->getAxisActionsByDevice( "Mouse" ) )
		{
			if ( mapping.inputID == MOUSE_POSITION_AXIS && hasStateChanged( MOUSE_POSITION_AXIS ) )
				handleAxisAction( _inputSystem, m_vdID, mapping.action, mapping.direction, m_state.position );

			if ( mapping.inputID == MOUSE_MOTION_AXIS && hasStateChanged( MOUSE_MOTION_AXIS ) )
			{
				double dt = wv::Application::getSingleton()->getDeltaTime(); // mouse movement needs to be un-scaled by delta time

				// TODO: replace 200 with mouse sensitivity, inversely proportional
				handleAxisAction( _inputSystem, m_vdID, mapping.action, mapping.direction, wv::Vector2f{ m_state.motion } / dt / 200.0f );
			}
			
		}
	}
}
