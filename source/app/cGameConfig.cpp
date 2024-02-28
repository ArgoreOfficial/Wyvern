#include "cGameConfig.h"

#include <wv/Core/cApplication.h>
#include <wv/Managers/cContentManager.h>
#include <wv/Graphics/cModel.h>

wv::sApplicationConfig cGameConfig::config( void )
{
	wv::sApplicationConfig config;

	config.name = "Game";
	config.version = "v1.0";
	config.title = "Wyvern Game";

	return config;
}

void cGameConfig::debugInit( void )
{
	m_model = wv::cContentManager::getInstance().getModel( "res/models/sponza-gltf/glTF/Sponza.gltf" );

	m_model->transform.scale = { 0.008f, 0.008f, 0.008f }; // TODO: fix
}

void cGameConfig::debugUpdate( double _delta_time )
{
	
}

void cGameConfig::debugRender( void )
{
	m_model->render();
}
