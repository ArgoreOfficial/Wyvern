#include "axis_action.h"

#include <wv/debug/log.h>

void wv::AxisAction::handleKeyboardEvent( uint32_t _scancode, bool _keyDown )
{
	if ( !isScancodeBound( _scancode ) ) return;

	double value = _keyDown ? 1.0 : -1.0;

	for ( auto& mapping : m_scancodeMappings )
	{
		if ( mapping.scancode != _scancode ) continue;

		switch ( mapping.direction )
		{
		case AxisDirection::HORIZONTAL_POSITIVE: 
			m_rawX += value; break;
		case AxisDirection::HORIZONTAL_NEGATIVE: 
			m_rawX -= value; break;
		case AxisDirection::VERTICAL_POSITIVE: 
			m_rawY += value; break;
		case AxisDirection::VERTICAL_NEGATIVE: 
			m_rawY -= value; break;
		}

		m_rawX = wv::Math::clamp( m_rawX, -1.0, 1.0 );
		m_rawY = wv::Math::clamp( m_rawY, -1.0, 1.0 );

		m_value.x = m_rawX;
		m_value.y = m_rawY;
		
		if( m_rawX != 0.0 || m_rawY != 0.0 )
			m_value.normalize();

		wv::Debug::Print( "%s: %f, %f\n", m_name.c_str(), m_value.x, m_value.y );

		break;
	}
}

void wv::AxisAction::handleControllerEvent( uint32_t _button, bool _buttonDown )
{
}

void wv::AxisAction::bindScancode( uint32_t _scancode, AxisDirection _direction )
{
	if ( isScancodeBound( _scancode ) ) 
		return;
	
	m_boundScancodes.insert( _scancode );
	m_scancodeMappings.push_back( { _direction, _scancode } );
	
	m_requiresRemapping = true;
}

void wv::AxisAction::unbindScancode( uint32_t _scancode ) 
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