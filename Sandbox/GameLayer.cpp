#include "GameLayer.h"
#include "FirstWindow.h"
#include "SecondWindow.h"

#include <Wyvern/Filesystem/cFilesystem.h>
#include <Wyvern/Managers/cAssetManager.h>

void GameLayer::start()
{
	wv::Events::cKeyDownEvent::hook<GameLayer>( &GameLayer::handleKeyInput, this );

	m_scene.add( new FirstWindow() );
	m_scene.add( new SecondWindow() );

	wv::cApplication::getViewport().clear( 0x222233FF );
}

void GameLayer::update( double _deltaTime )
{
	wv::cApplication::getViewport().clear( wv::Color::PacificBlue );
	wv::cApplication::getScene().update( _deltaTime );
}

void GameLayer::drawUI()
{
	wv::cApplication::getScene().drawUI();
}

void GameLayer::handleKeyInput( wv::Events::cKeyDownEvent& _event )
{
	// TODO: Create input manager
	if ( _event.getKeyCode() == 'B' )
	{
		WV_DEBUG( "B was pressed" );
	}
}
