#pragma once

#include <wv/input/action.h>

#include <set>

namespace wv {

class ButtonAction : public IAction
{
	WV_REFLECT_TYPE( ButtonAction )
public:
	ButtonAction( const std::string& _name ) : IAction( _name ) { }

	virtual void handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown ) override {
		if ( !isScancodeBound( _scancode ) ) return;
		updateState( _keyDown );
	}

	virtual void handleControllerEvent( wv::ControllerButton _button, bool _buttonDown ) override {
		if ( !isControllerButtonBound( _button ) ) return;
		updateState( _buttonDown );
	}

	virtual bool isBoundToKeyboard() const override { return !m_boundScancodes.empty(); }
	virtual bool isBoundToController() const override { return !m_boundControllerButtons.empty(); }
	virtual bool isBoundToMouse() const override { return false; }

	inline bool isScancodeBound( wv::Scancode _scancode ) const { return m_boundScancodes.contains( _scancode ); }
	inline bool isControllerButtonBound( wv::ControllerButton _button ) const { return m_boundControllerButtons.contains( _button ); }

	inline void bindScancode( wv::Scancode _scancode ) {
		if ( isScancodeBound( _scancode ) ) return;
		m_boundScancodes.insert( _scancode );
		m_requiresRemapping = true;
	}

	inline void unbindScancode( wv::Scancode _scancode ) {
		if ( !isScancodeBound( _scancode ) ) return;
		m_boundScancodes.erase( _scancode );
		m_requiresRemapping = true;
	}

	inline void bindControllerButton( wv::ControllerButton _button ) {
		if ( isControllerButtonBound( _button ) ) return;
		m_boundControllerButtons.insert( _button );
		m_requiresRemapping = true;
	}

	inline void unbindControllerButton( wv::ControllerButton _button ) {
		if ( !isControllerButtonBound( _button ) ) return;
		m_boundControllerButtons.erase( _button );
		m_requiresRemapping = true;
	}

private:
	void updateState( bool _newState );
	
	std::set<wv::Scancode> m_boundScancodes;
	std::set<wv::ControllerButton> m_boundControllerButtons;
	bool m_state = false;
};

}