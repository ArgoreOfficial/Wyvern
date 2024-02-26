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
}

void cEditorConfig::debugUpdate( double _delta_time )
{
	
}

void cEditorConfig::debugRender( void )
{
	ImGui::DockSpaceOverViewport();

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
	ImGui::Begin( "Viewport" );

	wv::cFramebuffer* viewbuffer = wv::cRenderer::getInstance().getViewbuffer();

	ImVec2        pos = ImGui::GetCursorScreenPos();
	ImVec2        window_size = ImGui::GetContentRegionAvail();
	
	ImGui::GetWindowDrawList()->AddImage(
		(void*)viewbuffer->m_framebuffer_object.textures[ 0 ].handle,
		ImVec2( pos.x, pos.y ),
		ImVec2( pos.x + window_size.x, pos.y + window_size.y ),
		ImVec2( 0, 1 ),
		ImVec2( 1, 0 )
	);

	wv::cRenderer::getInstance().onResize( window_size.x, window_size.y );

	ImGui::End();
}

void cEditorConfig::debugViewbufferEnd( void )
{


}
