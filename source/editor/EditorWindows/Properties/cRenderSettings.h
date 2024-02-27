#pragma once

#include "../iEditorWindow.h"

class cRenderSettings : public iEditorWindow
{

public:
	 cRenderSettings( void );
	~cRenderSettings( void );

	void render( void ) override;

private:

	bool m_msaa = true;

};