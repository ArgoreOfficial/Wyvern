#pragma once

#include <wv/Decl.h>
#include <imgui.h>

class iEditorWindow 
{
WV_DECLARE_INTERFACE( iEditorWindow )

public:

	virtual void render( void ) = 0;

};