#include "editor_app_state.h"

#include <3ds/3ds_remote_target.h>

#include <wv/debug/log.h>
#include <wv/engine.h>
#include <wv/memory/memory.h>

#include <wv/editor/target_manager_window.h>

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#endif

EditorAppState::EditorAppState()
{
	m_modeCombo      = Combo{ "##modes", { "Debug", "Debug-nomt", "Release", "Package"} };
	m_platformCombo  = Combo{ "##plats", { "3DS", "PSP2" } };
	m_runComboButton = ComboButton{ "##run_button", { "Local Debugger", "Remote Debugger", "Remote Run" } };

	m_editorWindows.push_back( WV_NEW( wv::TargetManagerWindow ) );
}

void EditorAppState::terminate()
{
	IAppState::terminate();

	wv::JobSystem* pJobSystem = wv::Engine::get()->m_pJobSystem;

	for ( auto w : m_editorWindows )
		WV_FREE( w );
	m_editorWindows.clear();

	if( m_buildFence )
		pJobSystem->waitAndDeleteFence( m_buildFence );
}

void EditorAppState::onDraw( wv::IDeviceContext* _pContext, wv::IGraphicsDevice* _pDevice )
{
	IAppState::onDraw( _pContext, _pDevice );

#ifdef WV_SUPPORT_IMGUI
	if( ImGui::BeginMainMenuBar() )
	{
		drawMenuBar();
		ImGui::EndMainMenuBar();
	}

	ImGuiViewportP* viewport = (ImGuiViewportP*)(void*)ImGui::GetMainViewport();
	
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();
	
	if ( ImGui::BeginViewportSideBar( "##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags ) )
	{
		if ( ImGui::BeginMenuBar() )
		{
			drawBuildWindow();
			m_runComboButton.draw();
			ImGui::EndMenuBar();
		}
	}

	if ( ImGui::BeginViewportSideBar( "##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags ) )
	{
		if ( ImGui::BeginMenuBar() )
		{
			ImGui::Text( "Happy status bar" );
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
#endif
}

void EditorAppState::drawMenuBar()
{
#ifdef WV_SUPPORT_IMGUI
	if ( ImGui::BeginMenu( "File" ) )
	{
		if ( ImGui::MenuItem( "Create" ) ) { WV_WARNING( "Unimplemented\n" ); }
		if ( ImGui::MenuItem( "Open", "Ctrl+O" ) ) { WV_WARNING( "Unimplemented\n" ); }
		if ( ImGui::MenuItem( "Save", "Ctrl+S" ) ) { WV_WARNING( "Unimplemented\n" ); }
		if ( ImGui::MenuItem( "Save as.." ) ) { WV_WARNING( "Unimplemented\n" ); }
		ImGui::EndMenu();
	}

	if ( ImGui::BeginMenu( "Build" ) )
	{
		ImGui::BeginDisabled( m_isBuilding3DS );
		if ( ImGui::MenuItem( "Build" ) )
			buildPlatform();

		if ( ImGui::MenuItem( "Build & Run" ) )
			buildAndRun();
		ImGui::EndDisabled();

		if ( ImGui::MenuItem( "Target Manager" ) ) { WV_WARNING( "Unimplemented\n" ); }
		ImGui::EndMenu();
	}

	if ( ImGui::BeginMenu( "View" ) )
	{
		for ( auto w : m_editorWindows )
		{
			std::string name = w->getName() + "##view";
			if ( ImGui::MenuItem( name.c_str() ) )
				w->toggle();
		}

		ImGui::EndMenu();
	}

	for ( auto w : m_editorWindows )
		if ( w->isOpen() )
			w->draw();

#endif
}

void EditorAppState::drawBuildWindow()
{
#ifdef WV_SUPPORT_IMGUI
	ImGui::SetNextItemWidth( ImGui::CalcTextSize( "255.255.255.255 " ).x );
	ImGui::InputText( "##ipinput", m_targetAddressStr, 16, ImGuiInputTextFlags_CharsDecimal );

	ImGui::SetNextItemWidth( ImGui::CalcTextSize( "65535 " ).x );
	ImGui::InputInt( "##portinput", &m_targetPort, 0 );

	m_platformCombo.draw( 100.0f );
	m_modeCombo.draw( 100.0f );
#endif
}

void EditorAppState::buildPlatform()
{
	if( m_isBuilding3DS )
		return;

	m_isBuilding3DS = true;

	wv::JobSystem* pJobSystem = wv::Engine::get()->m_pJobSystem;

	if( m_buildFence == nullptr )
		m_buildFence = pJobSystem->createFence();

	wv::Job::JobFunction_t fptr = [ & ]( void* )
		{
			wv::RemoteTarget3DS target( m_targetAddressStr, m_targetPort );

			wv::Debug::Print( "Building...\n" );
			if ( target.buildExecutable( m_modeCombo.currentOption ) )
				wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Build Failed\n" );
			
			m_isBuilding3DS = false;
		};

	wv::Job* job = pJobSystem->createJob( fptr, m_buildFence );
	pJobSystem->submit( { job } );
}

void EditorAppState::buildAndRun()
{
	buildPlatform();

	wv::JobSystem* pJobSystem = wv::Engine::get()->m_pJobSystem;
	wv::Job::JobFunction_t fptr = [ & ]( void* )
		{
			wv::Engine::get()->m_pJobSystem->waitForFence( m_buildFence );
			std::string launchFile = "Sandbox_" + std::string( m_modeCombo.currentOption ) + "_arm_3ds";

			wv::Debug::Print( "Launching\n" );
			wv::RemoteTarget3DS target( m_targetAddressStr, m_targetPort );

			if( target.remoteLaunchExecutable( launchFile, {} ) )
				wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Run Failed\n" );
			else
				wv::Debug::Print( "Launched on %s:%i\n", m_targetAddressStr, m_targetPort );

			/// TODO
			// cmd : arm-none-eabi-gdb Sandbox_Release_arm_3ds.elf
			// cmd : target remote 192.168.0.160:4003
		};

	wv::Job* job = pJobSystem->createJob( fptr );
	pJobSystem->submit( { job } );
}

Combo::Combo( const char* _name, const std::vector<const char*>& _options )
{
	name = _name;
	options = _options;
	currentOption = options[ 0 ];
}

int Combo::draw( float _width )
{
	size_t selected = 0;

#ifdef WV_SUPPORT_IMGUI
	if( _width > 0 )
		ImGui::SetNextItemWidth( _width );

	if( ImGui::BeginCombo( name, currentOption ) )
	{
		for( size_t i = 0; i < options.size(); i++ )
		{
			bool is_selected = ( currentOption == options[ i ] );

			if( ImGui::Selectable( options[ i ], is_selected ) )
			{
				currentOption = options[ i ];
				selected = i;
			}

			if( is_selected )
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
#endif

	currentOptionIndex = selected;
	return selected;
}

ComboButton::ComboButton( const char* _name, const std::vector<const char*>& _options )
{
	name = _name;
	options = _options;
	currentOption = options[ 0 ];
}

int ComboButton::draw( float _width )
{
#ifdef WV_SUPPORT_IMGUI
	std::string buildButtonStr = currentOption + std::string{ "##" } + std::string{ name };
	ImGui::Button( buildButtonStr.c_str() );

	float x = ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x;
	ImGui::SetCursorPosX( x );
	
	if ( ImGui::BeginCombo( name, 0, ImGuiComboFlags_NoPreview | ImGuiComboFlags_PopupAlignLeft ) )
	{
		for ( size_t i = 0; i < options.size(); i++ )
		{
			bool is_selected = ( currentOption == options[ i ] );

			if ( ImGui::Selectable( options[ i ], is_selected ) )
				currentOption = options[ i ];
			
			if ( is_selected )
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	return 0;
#endif
}
