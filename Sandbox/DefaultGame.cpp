#include "DefaultGame.h"

void DefaultGame::load()
{
	
}

void DefaultGame::start()
{
	
}

void DefaultGame::update( float _deltaTime )
{
	float time = WV::Application::getTime();
	
}

void DefaultGame::draw()
{

	// imgui
	if ( ImGui::Begin("Wyvern") )
	{
		ImGui::Text( "Welcome to Wyvern!" );

		ImGui::SliderFloat( "I'm a Slider!", &m_slider, 0.0f, 5.0f );
	}
	ImGui::End();

}