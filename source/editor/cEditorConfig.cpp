#include "cEditorConfig.h"

#include <wv/Core/cApplication.h>
#include <wv/Managers/cContentManager.h>
#include <wv/Graphics/cModel.h>

wv::sApplicationConfig cEditorConfig::config( void )
{
	wv::sApplicationConfig config;

	config.name    = "Editor";
	config.version = "v1.0";
	config.title   = "Wyvern Editor";

	return config;
}

void cEditorConfig::debugInit( void )
{
	m_model = wv::cContentManager::getInstance().getModel( "res/models/sponza-gltf/glTF/Sponza.gltf" );

	m_model->transform.scale = { 0.008f, 0.008f, 0.008f }; // TODO: fix
}

void cEditorConfig::debugUpdate( double _delta_time )
{
	
}

void cEditorConfig::debugRender( void )
{
	m_model->render();
}
