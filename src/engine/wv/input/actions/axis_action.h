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

	virtual void handleKeyboardEvent( uint32_t _scancode, bool _keyDown ) override;
	virtual void handleControllerEvent( uint32_t _button, bool _buttonDown ) override;

	virtual bool isBoundToKeyboard() const override { return !m_boundScancodes.empty(); }
	virtual bool isBoundToController() const override { return !m_boundControllerButtons.empty(); }
	virtual bool isBoundToMouse() const override { return false; }

	inline bool isScancodeBound( uint32_t _scancode ) const { return m_boundScancodes.contains( _scancode ); }
	inline bool isControllerButtonBound( uint32_t _button ) const { return m_boundControllerButtons.contains( _button ); }

	void bindScancode( uint32_t _scancode, AxisDirection _direction );

	void bindScancodes( uint32_t _posX, uint32_t _negX, uint32_t _posY, uint32_t _negY ) {
		bindScancode( _posY, wv::AxisAction::AxisDirection::VERTICAL_NEGATIVE );
		bindScancode( _negY, wv::AxisAction::AxisDirection::VERTICAL_POSITIVE );
		bindScancode( _negX, wv::AxisAction::AxisDirection::HORIZONTAL_NEGATIVE );
		bindScancode( _posX, wv::AxisAction::AxisDirection::HORIZONTAL_POSITIVE );
	}

	void unbindScancode( uint32_t _scancode );

//	inline void bindControllerButton( uint32_t _button, AxisDirection _direction ) {
//		if ( isControllerButtonBound( _button ) ) return;
//		m_boundControllerButtons.insert( _button );
//		m_requiresRemapping = true;
//	}
//
//	inline void unbindControllerButton( uint32_t _button ) {
//		if ( !isControllerButtonBound( _button ) ) return;
//		m_boundControllerButtons.erase( _button );
//		m_requiresRemapping = true;
//	}

private:
	struct ScancodeMapping { 
		AxisDirection direction;
		uint32_t scancode; 
	};

	std::set<uint32_t> m_boundScancodes;
	std::set<uint32_t> m_boundControllerButtons;

	std::vector<ScancodeMapping> m_scancodeMappings;

	double m_rawX = 0.0;
	double m_rawY = 0.0;
	wv::Vector2<double> m_value;

	bool m_isDown = false;

};

}