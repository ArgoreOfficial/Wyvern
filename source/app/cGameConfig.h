#pragma once

#include <wv/Core/iApplicationConfig.h>

namespace wv { class cModel; }

class cGameConfig : public wv::iApplicationConfig
{
public:
     cGameConfig( void ) { }
    ~cGameConfig( void ) { }

	wv::sApplicationConfig config( void ) override;

	void debugInit  ( void ) override;
	void debugUpdate( double _delta_time ) override;
	void debugRender( void ) override;
	
private:

	wv::cModel* m_model;

};