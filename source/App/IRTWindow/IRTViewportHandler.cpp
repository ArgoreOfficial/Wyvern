#include "IRTViewportHandler.h"

#include <wv/Device/GraphicsDevice.h>
#include <wv/RenderTarget/RenderTarget.h>

#include <imgui.h>

bool cIRTViewportHandler::shouldRecreate( void )
{
	if( !m_pRenderTarget )
		return true;

    return m_pRenderTarget->width  != m_viewportSize.x || 
           m_pRenderTarget->height != m_viewportSize.y;
}

void cIRTViewportHandler::create( wv::iGraphicsDevice* _pGraphics )
{
	wv::TextureDesc fbTex;
	fbTex.width  = m_viewportSize.x;
	fbTex.height = m_viewportSize.y;

	wv::RenderTargetDesc ibDesc;
	ibDesc.width  = fbTex.width;
	ibDesc.height = fbTex.height;
	ibDesc.numTextures = 1;
	ibDesc.textureDescs = &fbTex;
	m_pRenderTarget = _pGraphics->createRenderTarget( &ibDesc );
}

void cIRTViewportHandler::draw( wv::iGraphicsDevice* _pGraphics )
{
	if( m_pRenderTarget == nullptr )
		return;

	ImGui::Begin( "Viewport" );

	m_viewportSize.x = ( int )ImGui::GetContentRegionAvail().x;
	m_viewportSize.y = ( int )ImGui::GetContentRegionAvail().y;

	if( m_viewportSize.x <= 0 ) m_viewportSize.x = 1;
	if( m_viewportSize.y <= 0 ) m_viewportSize.y = 1;

	ImVec2 pos = ImGui::GetCursorScreenPos();
	
	ImGui::GetWindowDrawList()->AddImage(
		( void* )m_pRenderTarget->fbHandle,
		ImVec2( pos.x, pos.y ),
		ImVec2( pos.x + m_viewportSize.x, pos.y + m_viewportSize.y ),
		ImVec2( 0, 1 ),
		ImVec2( 1, 0 )
	);

	ImGui::End();

	sceneGraphWindow.draw( _pGraphics );
}
