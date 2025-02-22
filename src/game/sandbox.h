#pragma once

#include <wv/app.h>

class Sandbox : public wv::IApplication
{
public:
	Sandbox() { }

	// Inherited via IApplication
	bool create ( void ) override;
	void run    ( void ) override;
	void destroy( void ) override;
};