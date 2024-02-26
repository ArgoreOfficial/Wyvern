#pragma once

#include <wv/Core/iApplicationConfig.h>

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
	
private:

	wv::cModel* m_model;

};