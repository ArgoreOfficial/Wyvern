#pragma once

#include <string>

namespace wv {

class IEditorWindow
{
public:
	constexpr IEditorWindow( const char* _name ) : 
		m_name{ _name }
	{ }

	virtual void draw() = 0;
	const std::string getName() {
		return m_name;
	}

	bool isOpen( void ) { return m_open;    }
	void open  ( void ) { m_open = true;    }
	void close ( void ) { m_open = false;   }
	void toggle( void ) { m_open = !m_open; }

private:
	bool m_open = false;
	const std::string m_name;
};

}