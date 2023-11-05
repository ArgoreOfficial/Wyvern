#include "GameLayer.h"
#include "PropertiesWindow.h"
#include "MenuBarWindow.h"
#include "PreviewWindow.h"
#include "SceneGraphWindow.h"

#include <Wyvern/Filesystem/Filesystem.h>
#include <Wyvern/Managers/AssetManager.h>

void GameLayer::start()
{
	WV::Events::KeyDownEvent::hook<GameLayer>( &GameLayer::handleKeyInput, this );

	m_camera.setFOV( 60.0f );
	m_camera.setPosition( 0, 0, -1.0f );
	WV::Application::setActiveCamera( &m_camera );

	m_scene.add( new MenuBarWindow() );
	m_scene.add( new PropertiesWindow() );
	m_scene.add( new PreviewWindow() );
	m_scene.add( new SceneGraphWindow() );

	WV::Application::getWindow().setClearColour( 0x222233FF );
}

void GameLayer::update( double _deltaTime )
{
	WV::Application::getWindow().setClearColour( 0x666699FF );
	WV::Application::getScene().update( _deltaTime );
}

void GameLayer::drawUI()
{
	ImGui::DockSpaceOverViewport( ImGui::GetMainViewport() );
	WV::Application::getScene().drawUI();
}

void GameLayer::handleKeyInput( WV::Events::KeyDownEvent& _event )
{
	if ( _event.getKeyCode() == 'B' )
	{
		WVDEBUG( "B was pressed" );
	}
}
