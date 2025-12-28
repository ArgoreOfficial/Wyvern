#pragma once

#include <wv/input/action.h>

#include <set>

namespace wv {

class ButtonAction : public IAction
{
	WV_REFLECT_TYPE( ButtonAction )
public:
	ButtonAction( const std::string& _name ) : IAction( _name ) { }

	virtual void handleKeyboardEvent( uint32_t _scancode, bool _keyDown ) override;
	virtual void handleControllerEvent( uint32_t _button, bool _buttonDown ) override;

	virtual bool isBoundToKeyboard() const override { return !m_boundScancodes.empty(); }
	virtual bool isBoundToController() const override { return !m_boundControllerButtons.empty(); }
	virtual bool isBoundToMouse() const override { return false; }

	inline bool isScancodeBound( uint32_t _scancode ) const { return m_boundScancodes.contains( _scancode ); }
	inline bool isControllerButtonBound( uint32_t _button ) const { return m_boundControllerButtons.contains( _button ); }

	inline void bindScancode( uint32_t _scancode ) {
		if ( isScancodeBound( _scancode ) ) return;
		m_boundScancodes.insert( _scancode );
		m_requiresRemapping = true;
	}

	inline void unbindScancode( uint32_t _scancode ) {
		if ( !isScancodeBound( _scancode ) ) return;
		m_boundScancodes.erase( _scancode );
		m_requiresRemapping = true;
	}

	inline void bindControllerButton( uint32_t _button ) {
		if ( isControllerButtonBound( _button ) ) return;
		m_boundControllerButtons.insert( _button );
		m_requiresRemapping = true;
	}

	inline void unbindControllerButton( uint32_t _button ) {
		if ( !isControllerButtonBound( _button ) ) return;
		m_boundControllerButtons.erase( _button );
		m_requiresRemapping = true;
	}

private:

	std::set<uint32_t> m_boundScancodes;
	std::set<uint32_t> m_boundControllerButtons;
	bool m_isDown = false;

};

}