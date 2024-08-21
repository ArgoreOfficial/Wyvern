#pragma once

#include <wv/Engine/Application.h>

namespace wv { class cEngine; }
namespace wv { class cSceneRoot; }
namespace wv { class cFileSystem; }

class cSandbox : public wv::iApplication
{
public:
	cSandbox() { }

	// Inherited via iApplication
	bool create ( void ) override;
	void run    ( void ) override;
	void destroy( void ) override;

	wv::cSceneRoot* setupScene( wv::cFileSystem* _pFileSystem );

private:

	wv::cEngine* m_pEngine = nullptr;
};