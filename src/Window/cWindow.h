#pragma once

#include "../engine.h"

class cWindow
{
public:
	 cWindow();
	~cWindow();

	bool create( unsigned int _width, unsigned int _height, const char* _title );

	void startFrame( void );
	void endFrame  ( void );

	bool shouldClose( void ) const { return glfwWindowShouldClose( m_window_object ); }

	GLFWwindow* const getWindowObject( void ) { return m_window_object; }
	unsigned int getWidth ( void ) const { return m_width; }
	unsigned int getHeight( void ) const { return m_height; }

private:

	GLFWwindow* m_window_object = nullptr;
	unsigned int m_width = 0;
	unsigned int m_height = 0;

};
