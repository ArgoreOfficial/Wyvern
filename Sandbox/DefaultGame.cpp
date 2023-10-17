#include "DefaultGame.h"

void DefaultGame::load()
{
	
}

void DefaultGame::start()
{
	WV::Events::KeyDownEvent::hook<DefaultGame>( &DefaultGame::handleKeyInput, this );
}

void DefaultGame::update( float _deltaTime )
{
	float time = WV::Application::getTime();
	
}

void DefaultGame::draw()
{

	// imgui
	if ( m_showWindow )
	{
		if ( ImGui::Begin("Wyvern") )
		{
			ImGui::Text( "Welcome to Wyvern!" );

			ImGui::SliderFloat( "I'm a Slider!", &m_slider, 0.0f, 5.0f );
		}
		ImGui::End();
	}

}

void DefaultGame::handleKeyInput( WV::Events::KeyDownEvent& _event )
{
	if ( _event.getKeyCode() == 'B' )
	{
		m_showWindow = !m_showWindow;
	}
}
