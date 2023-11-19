#include "GameLayer.h"
#include "PropertiesWindow.h"
#include "MenuBarWindow.h"
#include "PreviewWindow.h"
#include "SceneGraphWindow.h"

#include <Wyvern/Filesystem/cFilesystem.h>
#include <Wyvern/Managers/cAssetManager.h>

void GameLayer::start()
{
	WV::Events::cKeyDownEvent::hook<GameLayer>( &GameLayer::handleKeyInput, this );

	m_camera.setFOV( 60.0f );
	m_camera.setPosition( 0, 0, -1.0f );
	WV::cApplication::setActiveCamera( &m_camera );

	m_scene.add( new MenuBarWindow() );
	m_scene.add( new PropertiesWindow() );
	m_scene.add( new PreviewWindow() );
	m_scene.add( new SceneGraphWindow() );

	WV::cApplication::getWindow().setClearColour( 0x222233FF );
}

void GameLayer::update( double _deltaTime )
{
	WV::cApplication::getWindow().setClearColour( 0x666699FF );
	WV::cApplication::getScene().update( _deltaTime );
}

void GameLayer::drawUI()
{
	ImGui::DockSpaceOverViewport( ImGui::GetMainViewport() );
	WV::cApplication::getScene().drawUI();
}

void GameLayer::handleKeyInput( WV::Events::cKeyDownEvent& _event )
{
	if ( _event.getKeyCode() == 'B' )
	{
		WV_DEBUG( "B was pressed" );
	}
}
