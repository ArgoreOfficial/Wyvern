#pragma once

#include "../iEditorWindow.h"

class cViewportWindow : public iEditorWindow
{

public:
	 cViewportWindow( void );
	~cViewportWindow( void );

	void render( void ) override;

};