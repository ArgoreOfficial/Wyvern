#include "RewindSettingsWindow.h"

#include <imgui.h>

psq::cRewindSettingsWindow::cRewindSettingsWindow( const wv::UUID& _uuid, const std::string& _name ) :
	wv::iSceneObject( _uuid, _name )
{
	m_recorder.startRecordingVariable( &m_windowPos );

	m_recorder.setEnabled( m_rewindImgui );
}

void psq::cRewindSettingsWindow::updateImpl( double _deltaTime )
{
	m_recorder.setRewinding ( cRewindSettingsWindow::isRewinding );
	m_recorder.setFrameSpeed( cRewindSettingsWindow::rewindSpeed );

	if( !cRewindSettingsWindow::worldIsPaused )
	{
		currentFrame++;

		if( cRewindSettingsWindow::currentFrame > m_maxFrames )
			m_maxFrames++;

		m_recorder.update();
	}

}

void psq::cRewindSettingsWindow::drawImpl( wv::iDeviceContext* _context, wv::iGraphicsDevice* _device )
{
	ImVec2 pos{ m_windowPos.x, m_windowPos.y };
	
	if( m_rewindImgui )
	{
		if( cRewindSettingsWindow::isRewinding || m_firstFrame )
		{
			ImGui::SetNextWindowPos( pos );
			m_firstFrame = false;
		}
	}

	bool wopen = ImGui::Begin( m_name.c_str() );
	pos = ImGui::GetWindowPos();



	if ( wopen )
	{
		ImGui::Checkbox( "##Pause", &cRewindSettingsWindow::worldIsPaused );
		
		if( !cRewindSettingsWindow::worldIsPaused )
			ImGui::BeginDisabled();

		ImGui::SameLine();
		ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
		
		float minY = ImGui::GetWindowContentRegionMin().y + ImGui::GetWindowPos().y;
		float maxY = ImGui::GetWindowContentRegionMax().y + ImGui::GetWindowPos().y;
		
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 4, ( maxY - minY ) * 0.5f - 7.0f } );
		ImGui::SliderInt( "##Frame", &cRewindSettingsWindow::currentFrame, 0.f, m_maxFrames );
		
		ImGui::PopStyleVar( 2 );

		if( !cRewindSettingsWindow::worldIsPaused )
			ImGui::EndDisabled();
	}
	ImGui::End();

	m_windowPos = { pos.x, pos.y };
}
