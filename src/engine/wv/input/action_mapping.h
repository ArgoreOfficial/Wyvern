#pragma once

#include <wv/reflection/reflection.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

namespace wv {

class IAction : public IReflectedType
{
	friend class ActionGroup;

	WV_REFLECT_TYPE( IAction )
public:
	IAction( const std::string& _name ) : m_name{ _name } { }

	virtual void handleKeyboardEvent( uint32_t _scancode, bool _keyDown ) = 0;
	virtual bool isBoundToKeyboard() const = 0;
	
	bool requiresRemapping() const { return m_requiresRemapping; };

protected:
	std::string m_name; 
	bool m_requiresRemapping = true;
};

class ButtonAction : public IAction
{
	WV_REFLECT_TYPE( ButtonAction )
public:
	ButtonAction( const std::string& _name ) : IAction( _name ) { }

	virtual void handleKeyboardEvent( uint32_t _scancode, bool _keyDown ) override;
	virtual bool isBoundToKeyboard() const override { return !m_boundScancodes.empty(); }

	inline bool isScancodeBound( uint32_t _scancode ) const { return m_boundScancodes.contains( _scancode ); }
	
	inline void clearBoundScancodes() { 
		m_boundScancodes.clear(); 
		m_requiresRemapping = true;
	}

	inline void bindScancode( uint32_t _scancode ) {
		if ( isScancodeBound( _scancode ) )
			return;

		m_boundScancodes.insert( _scancode );
		m_requiresRemapping = true;
	}

	inline void unbindScancode( uint32_t _scancode ) {
		if ( !isScancodeBound( _scancode ) )
			return;

		m_boundScancodes.erase( _scancode );
		m_requiresRemapping = true;
	}

private:

	std::set<uint32_t> m_boundScancodes;
	bool m_isDown = false;

};

class ActionGroup
{
public:
	ActionGroup( const std::string& _name ) 
		: m_name{ _name } 
	{ }
	
	~ActionGroup();

	inline bool isEnabled() const { return m_isEnabled; }

	void enable();
	void disable() {
		WV_ASSERT( m_isEnabled == false );
		m_isEnabled = false;
	}

	std::string getName() const { return m_name; }

	ButtonAction* createButtonAction( const std::string& _name );
	void destroyAction( const std::string& _name );

	void handleKeyboardEvent( uint32_t _scancode, bool _keyDown );

private:
	void buildActionMapping();

	std::string m_name;

	bool m_isEnabled = false;
	bool m_requiresRemapping = false;

	std::vector<IAction*> m_actions;
	std::unordered_map<std::string, IAction*> m_actionNameMap;

	std::vector<IAction*> m_keyboardBoundActions;
	std::vector<IAction*> m_gamepadBoundActions;
	std::vector<IAction*> m_mouseBoundActions;
};

}