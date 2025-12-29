#pragma once

#include <wv/input/action.h>
#include <wv/math/vector2.h>

#include <set>

namespace wv {

class AxisAction : public IAction
{
	WV_REFLECT_TYPE( AxisAction )
public:
	enum class AxisDirection
	{
		HORIZONTAL_POSITIVE = 0,
		HORIZONTAL_NEGATIVE,
		VERTICAL_POSITIVE,
		VERTICAL_NEGATIVE
	};

	AxisAction( const std::string& _name ) : IAction( _name ) { }

	virtual void handleKeyboardEvent( wv::Scancode _scancode, bool _keyDown ) override;
	virtual void handleControllerEvent( wv::ControllerButton _button, bool _buttonDown ) override;

	virtual bool isBoundToKeyboard() const override { return !m_boundScancodes.empty(); }
	virtual bool isBoundToController() const override { return !m_boundControllerButtons.empty(); }
	virtual bool isBoundToMouse() const override { return false; }

	inline bool isScancodeBound( wv::Scancode _scancode ) const { return m_boundScancodes.contains( _scancode ); }
	inline bool isControllerButtonBound( wv::ControllerButton _button ) const { return m_boundControllerButtons.contains( _button ); }

	void bindScancode( wv::Scancode _scancode, AxisDirection _direction );
	void unbindScancode( wv::Scancode _scancode );

	void bindScancodes( wv::Scancode _posX, wv::Scancode _negX, wv::Scancode _posY, wv::Scancode _negY ) {
		bindScancode( _posY, AxisDirection::VERTICAL_NEGATIVE );
		bindScancode( _negY, AxisDirection::VERTICAL_POSITIVE );
		bindScancode( _negX, AxisDirection::HORIZONTAL_NEGATIVE );
		bindScancode( _posX, AxisDirection::HORIZONTAL_POSITIVE );
	}

	wv::Vector2<double> getValue() const { return m_value; }

	void bindControllerButton( wv::ControllerButton _button, AxisDirection _direction );
	void unbindControllerButton( wv::ControllerButton _button );

	void bindControllerButtons( wv::ControllerButton _posX, wv::ControllerButton _negX, wv::ControllerButton _posY, wv::ControllerButton _negY ) {
		bindControllerButton( _posY, AxisDirection::VERTICAL_NEGATIVE );
		bindControllerButton( _negY, AxisDirection::VERTICAL_POSITIVE );
		bindControllerButton( _negX, AxisDirection::HORIZONTAL_NEGATIVE );
		bindControllerButton( _posX, AxisDirection::HORIZONTAL_POSITIVE );
	}

private:
	struct AxisMapping 
	{
		bool shouldActOn( bool _down ) {
			return !hasBeenActedOn || ( _down && wasPressedUp ) || ( !_down && wasPressedDown );
		}

		void actOn( bool _down ) {
			hasBeenActedOn = true;
			if ( _down ) { wasPressedUp = false; wasPressedDown = true; }
			else         { wasPressedUp = true;  wasPressedDown = false; }
		}

		AxisDirection direction;
		bool hasBeenActedOn = false;
		bool wasPressedDown = false;
		bool wasPressedUp = false;
	};

	struct ScancodeMapping : AxisMapping 
	{ 
		wv::Scancode scancode;
	};
	
	struct ControllerButtonMapping : AxisMapping 
	{ 
		wv::ControllerButton button;
	};

	void handleKeyButton( AxisDirection _direction, bool _down );

	std::set<wv::Scancode> m_boundScancodes;
	std::set<wv::ControllerButton> m_boundControllerButtons;

	std::vector<ScancodeMapping> m_scancodeMappings;
	std::vector<ControllerButtonMapping> m_controllerButtonMappings;

	double m_rawX = 0.0;
	double m_rawY = 0.0;
	wv::Vector2<double> m_value;

	bool m_isDown = false;

};

}