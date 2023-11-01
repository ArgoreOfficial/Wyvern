#include "GameLayer.h"
#include <Wyvern/Filesystem/Filesystem.h>
#include <Wyvern/Managers/AssetManager.h>

void GameLayer::start()
{
	WV::Events::KeyDownEvent::hook<GameLayer>( &GameLayer::handleKeyInput, this );
	m_camera.setFOV( 60.0f );
	m_camera.setPosition( 0, 0, -1.0f );
	WV::Application::setActiveCamera( &m_camera );

	// this returns an asset* with m_ready = false
	// all function calls will return until m_ready == true
	m_meshAsset = new WV::Mesh( "assets/logo.obj" );

	WV::Application::getWindow().setClearColour( 0x222233FF );
}

void GameLayer::update( double _deltaTime )
{
	WV::Application::getWindow().setClearColour( 0x666699FF );

	float time = WV::Application::getTime();

	m_camera.rotate( 0.0f, _deltaTime, 0.0f );
}

void GameLayer::drawUI()
{
	ImGui::DockSpaceOverViewport( ImGui::GetMainViewport() );

	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( "File" ) )
		{
			if ( ImGui::MenuItem( "Load", "CTRL+O" ) ) { }
			if ( ImGui::MenuItem( "Save", "CTRL+S" ) ) { }
			ImGui::EndMenu();
		}
		if ( ImGui::BeginMenu( "Edit" ) )
		{
			if ( ImGui::MenuItem( "Undo", "CTRL+Z" ) ) { }
			if ( ImGui::MenuItem( "Redo", "CTRL+Y", false, false ) ) { }  // Disabled item
			ImGui::Separator();
			if ( ImGui::MenuItem( "Cut", "CTRL+X" ) ) { }
			if ( ImGui::MenuItem( "Copy", "CTRL+C" ) ) { }
			if ( ImGui::MenuItem( "Paste", "CTRL+V" ) ) { }
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if ( ImGui::Begin( "Properties" ) )
	{

	}
	ImGui::End();
	
	if ( ImGui::Begin( "SceneGraph" ) )
	{

	}
	ImGui::End();

	if ( ImGui::Begin( "Preview" ) )
	{

	}
	ImGui::End();
}

void GameLayer::handleKeyInput( WV::Events::KeyDownEvent& _event )
{
	if ( _event.getKeyCode() == 'B' )
	{
		WVDEBUG( "B was pressed" );
	}
}
