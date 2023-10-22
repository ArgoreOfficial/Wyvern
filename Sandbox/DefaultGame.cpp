#include "DefaultGame.h"
#include <Wyvern/Filesystem/Filesystem.h>

#include <Wyvern/Managers/AssetManager.h>

void DefaultGame::start()
{
	WV::Events::KeyDownEvent::hook<DefaultGame>( &DefaultGame::handleKeyInput, this );
	WV::Application::getWindow()->setClearColour( 0x666699FF );

	m_camera.setFOV( 60.0f );
	m_camera.setPosition( 0, 0, -1.0f );
	WV::Application::setActiveCamera( &m_camera );

	// this returns an asset* with m_ready=false
	// all function calls will return until m_ready==true
	m_meshAsset = new WV::Mesh( "assets/logo.obj" );

	/*

	// --------- Goal --------- //

	// however, objects (entities) are prefered:
	Entity* entity = new Entity(); // all entities have a transform component by default
	entity->addComponent<Mesh>( "logo.obj" );

	// addComponent<T>() returns a T*
	Material* material entity->addComponent<Material>();
	material->albedo.texture = new Texture( "logo_fire_BaseColor" );
	material->roughness = 0.5f;

	entity->addComponent<MyCustomComponent>();

	// --------- Goal --------- //


	*/

	WV::Application::getWindow()->setClearColour( 0x222233FF );
}

void DefaultGame::update( double _deltaTime )
{
	WV::Application::getWindow()->setClearColour( 0x666699FF );

	float time = WV::Application::getTime();

	m_camera.rotate( 0.0f, _deltaTime, 0.0f );
	
}

void DefaultGame::draw3D()
{
	
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
