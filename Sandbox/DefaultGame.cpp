#include "DefaultGame.h"
#include <Wyvern/Filesystem/Filesystem.h>

#include <Wyvern/Managers/AssetManager.h>

void DefaultGame::load()
{
	WV::Application::getWindow()->setClearColour( 0x222233FF );

	WV::cAssetManager::load( "assets/logo_fire_BaseColor.png", WV::eAssetType::RAW );
	WV::cAssetManager::load( "assets/logo.obj", WV::eAssetType::MESH );
}


void DefaultGame::start()
{
	WV::Events::KeyDownEvent::hook<DefaultGame>( &DefaultGame::handleKeyInput, this );
	WV::Application::getWindow()->setClearColour( 0x666699FF );

	m_camera.setFOV( 60.0f );
	m_camera.setPosition( 0, 0, -1.0f );
	WV::Application::setActiveCamera( &m_camera );

	m_model = new WV::Model( *WV::cAssetManager::getMesh( "logo.obj" ) );
}

void DefaultGame::update( float _deltaTime )
{
	float time = WV::Application::getTime();

	m_camera.rotate( 0.0f, _deltaTime, 0.0f );
	
}


void DefaultGame::draw()
{
	m_model->submit( WV::Application::getWindow()->getAspect() );
}

void DefaultGame::drawUI()
{
	// ImGui::DockSpaceOverViewport( ImGui::GetMainViewport() );

	// ImGui::ShowDemoWindow();

	if ( m_showWindow )
	{
		if ( ImGui::Begin( "Wyvern" ) )
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
