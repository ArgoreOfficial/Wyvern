#include "RewindSettingsWindow.h"

#include <imgui.h>
#include <SDL2/SDL_keycode.h>

psq::cRewindSettingsWindow::cRewindSettingsWindow( const wv::UUID& _uuid, const std::string& _name ) :
	wv::iSceneObject( _uuid, _name )
{
	m_recorder.startRecordingVariable( &m_windowPos );

	m_recorder.setEnabled( m_rewindImgui );

	subscribeInputEvent();
}

void psq::cRewindSettingsWindow::onInputEvent( wv::InputEvent _event )
{
	if( _event.buttondown && _event.key == SDLK_SPACE ) 
		togglePause();
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

	bool wopen = ImGui::Begin( m_name.c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize );
	pos = ImGui::GetWindowPos();

	if ( wopen )
	{
		const char* buttonLabel = cRewindSettingsWindow::worldIsPaused ? "Play" : "Pause";
		if( ImGui::Button( buttonLabel ) ) togglePause();
		
		if( !cRewindSettingsWindow::worldIsPaused )
			ImGui::BeginDisabled();

		ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
		
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 4, 60 } );
		ImGui::SliderInt( "##Frame", &cRewindSettingsWindow::currentFrame, 0.f, m_maxFrames );
		ImGui::PopStyleVar();
		
		if( !cRewindSettingsWindow::worldIsPaused )
			ImGui::EndDisabled();
	}
	ImGui::End();

	m_windowPos = { pos.x, pos.y };
}
