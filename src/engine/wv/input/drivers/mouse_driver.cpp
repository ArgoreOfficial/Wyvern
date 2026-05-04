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
			if ( mapping.inputID >= MouseInput_Left && mapping.inputID <= MouseInput_X2 && hasStateChanged( (MouseInputs)mapping.inputID ) )
				handleTriggerAction( _inputSystem, m_vdID, mapping.action, m_state.buttonStates[ mapping.inputID ] );
			
			if ( mapping.inputID == MouseInput_ScrollDelta && hasStateChanged( (MouseInputs)mapping.inputID ) )
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
			if ( mapping.inputID >= MouseInput_Left && mapping.inputID <= MouseInput_X2 && hasStateChanged( (MouseInputs)mapping.inputID ) )
				handleValueAction( _inputSystem, m_vdID, mapping.action, m_state.buttonStates[ mapping.inputID ] ? 1.0f : 0.0f );
			
			if ( mapping.inputID == MouseInput_ScrollDelta && hasStateChanged( MouseInput_ScrollDelta ) )
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
			if ( mapping.inputID == MouseInput_PositionAxis && hasStateChanged( MouseInput_PositionAxis ) )
				handleAxisAction( _inputSystem, m_vdID, mapping.action, mapping.direction, m_state.position, group->isEnabled() );

			if ( mapping.inputID == MouseInput_MotionAxis && hasStateChanged( MouseInput_MotionAxis ) )
			{
				double dt = wv::Application::getSingleton()->getDeltaTime(); // mouse movement needs to be un-scaled by delta time

				// TODO: replace 200 with mouse sensitivity, inversely proportional
				handleAxisAction( _inputSystem, m_vdID, mapping.action, mapping.direction, wv::Vector2f{ m_state.motion } / dt / 200.0f, group->isEnabled() );
			}
			
		}
	}
}
