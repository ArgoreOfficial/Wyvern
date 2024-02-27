#pragma once

#include <wv/Core/iApplicationConfig.h>

#include "EditorWindows/Viewport/cViewportWindow.h"
#include "EditorWindows/Properties/cRenderSettings.h"

#include <vector>

namespace wv { class cModel; }

class cEditorConfig : public wv::iApplicationConfig
{
public:
	 cEditorConfig( void ) { }
    ~cEditorConfig( void ) { }

	wv::sApplicationConfig config( void ) override;

	void debugInit  ( void ) override;
	void debugUpdate( double _delta_time ) override;
	void debugRender( void ) override;
	
	void debugViewbufferBegin( void ) override;
	void debugViewbufferEnd  ( void ) override;

private:

	wv::cModel* m_model; // testing
	
	bool m_render_setting_msaa = true;

	cViewportWindow m_viewport_window;

	std::vector<iEditorWindow*> m_editor_windows;

};