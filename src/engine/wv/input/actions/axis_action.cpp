#include "axis_action.h"

#include <wv/debug/log.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::AxisAction::handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown )
{
	if ( !isScancodeBound( _scancode ) ) return;

	for ( auto& mapping : m_scancodeMappings )
	{
		if ( mapping.scancode != _scancode || !mapping.shouldActOn( _keyDown ) )
			continue;
		
		mapping.actOn( _keyDown );
		handleKeyButton( mapping.direction, _keyDown );
		
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::AxisAction::handleControllerEvent( uint32_t _button, bool _buttonDown )
{
	if ( !isControllerButtonBound( _button ) ) return;

	for ( auto& mapping : m_controllerButtonMappings )
	{
		if ( mapping.button != _button || !mapping.shouldActOn( _buttonDown ) )
			continue;

		mapping.actOn( _buttonDown );
		handleKeyButton( mapping.direction, _buttonDown );
		
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::AxisAction::bindScancode( wv::Scancode _scancode, AxisDirection _direction )
{
	if ( isScancodeBound( _scancode ) ) 
		return;
	
	ScancodeMapping mapping{};
	mapping.direction = _direction;
	mapping.scancode = _scancode;
	m_scancodeMappings.push_back( mapping );
	m_boundScancodes.insert( _scancode );

	m_requiresRemapping = true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::AxisAction::unbindScancode( wv::Scancode _scancode )
{
	if ( !isScancodeBound( _scancode ) ) 
		return;
	
	m_boundScancodes.erase( _scancode );
	for ( auto it = m_scancodeMappings.begin(); it != m_scancodeMappings.end(); )
	{
		if ( it->scancode != _scancode ) continue;
		m_scancodeMappings.erase( it );
		break;
	}

	m_requiresRemapping = true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::AxisAction::bindControllerButton( uint32_t _button, AxisDirection _direction )
{
	if ( isControllerButtonBound( _button ) ) 
		return;
	
	ControllerButtonMapping mapping{};
	mapping.direction = _direction;
	mapping.button = _button;
	m_controllerButtonMappings.push_back( mapping );
	m_boundControllerButtons.insert( _button );
	m_requiresRemapping = true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::AxisAction::unbindControllerButton( uint32_t _button )
{
	if ( !isControllerButtonBound( _button ) ) 
		return;
	
	m_boundControllerButtons.erase( _button );
	for ( auto it = m_controllerButtonMappings.begin(); it != m_controllerButtonMappings.end(); )
	{
		if ( it->button != _button ) continue;
		m_controllerButtonMappings.erase( it );
		break;
	}

	m_requiresRemapping = true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::AxisAction::handleKeyButton( AxisDirection _direction, bool _down )
{
	double value = _down ? 1.0 : -1.0;

	switch ( _direction )
	{
	case AxisDirection::HORIZONTAL_POSITIVE: m_rawX += value; break;
	case AxisDirection::HORIZONTAL_NEGATIVE: m_rawX -= value; break;
	case AxisDirection::VERTICAL_POSITIVE: m_rawY += value; break;
	case AxisDirection::VERTICAL_NEGATIVE: m_rawY -= value; break;
	}

	m_rawX = wv::Math::clamp( m_rawX, -1.0, 1.0 );
	m_rawY = wv::Math::clamp( m_rawY, -1.0, 1.0 );

	m_value.x = m_rawX;
	m_value.y = m_rawY;

	if ( m_rawX != 0.0 || m_rawY != 0.0 )
		m_value.normalize();
}
