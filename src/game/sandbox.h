#pragma once

#include <wv/app.h>

class cSandbox : public wv::iApplication
{
public:
	cSandbox() { }

	// Inherited via iApplication
	bool create ( void ) override;
	void run    ( void ) override;
	void destroy( void ) override;
};