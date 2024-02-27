#include "cViewportWindow.h"

#include <wv/Core/cRenderer.h>
#include <wv/Rendering/cFramebuffer.h>

cViewportWindow::cViewportWindow( void )
{

}

cViewportWindow::~cViewportWindow( void )
{

}

void cViewportWindow::render( void )
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
