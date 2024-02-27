#include "cEditorConfig.h"

#include <wv/Rendering/cFramebuffer.h>

#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>
#include <wv/Managers/cContentManager.h>
#include <wv/Graphics/cModel.h>

#include <imgui.h>

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

	m_editor_windows.push_back( new cRenderSettings() );
}

void cEditorConfig::debugUpdate( double _delta_time )
{
	
}

void cEditorConfig::debugRender( void )
{
	ImGui::DockSpaceOverViewport();

	for ( int i = 0; i < m_editor_windows.size(); i++ )
		m_editor_windows[ i ]->render();
	
	if ( ImGui::Begin( "Properties" ) )
	{

	}
	ImGui::End();


	if ( ImGui::Begin( "Scenegraph" ) )
	{

	}
	ImGui::End();


	if ( ImGui::Begin( "Asset Explorer" ) )
	{

	}
	ImGui::End();

	

	m_model->render();
}

void cEditorConfig::debugViewbufferBegin( void )
{
	m_viewport_window.render(); // special case
}

void cEditorConfig::debugViewbufferEnd( void )
{

}
