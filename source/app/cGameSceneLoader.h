#pragma once

#include <wv/Scene/cSceneLoader.h>

class cGameSceneLoader : public wv::cSceneLoader
{
public:
     cGameSceneLoader( void ) : wv::cSceneLoader( "game" ) { }
    ~cGameSceneLoader( void ) { }

    void load( wv::cScene* _scene ) override;
};