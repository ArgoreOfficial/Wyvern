#include "editor_interface_system.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

void wv::EditorInterfaceSystem::onInitialize()
{
	setEditorRenderEnabled( true );
}

void wv::EditorInterfaceSystem::onUpdate()
{
	m_timeSinceFPSUpdate += deltaTime;
	m_framesSinceFPSUpdate++;

	if ( m_timeSinceFPSUpdate > 0.5 )
	{
		double averageDt = m_timeSinceFPSUpdate / m_framesSinceFPSUpdate;

		if ( averageDt != 0 )
			m_averageFPS = 1.0 / averageDt;

		m_timeSinceFPSUpdate -= 0.5;
		m_framesSinceFPSUpdate = 0;
	}
}

void wv::EditorInterfaceSystem::onEditorRender()
{
#ifdef WV_SUPPORT_IMGUI
	renderPrimaryMenuBar();
	renderSecondaryMenuBar();
	renderStatusBar();

//	if ( m_showRenderWorldWindow )
//		renderWorldWindow();

#endif
}


void wv::EditorInterfaceSystem::renderPrimaryMenuBar()
{
	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( "Tools" ) )
		{
//			if ( ImGui::MenuItem( "World" ) )
//				m_showRenderWorldWindow = !m_showRenderWorldWindow;
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}

void wv::EditorInterfaceSystem::renderSecondaryMenuBar()
{
	ImGuiViewportP* viewport = (ImGuiViewportP*)ImGui::GetMainViewport();

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();

	if ( ImGui::BeginViewportSideBar( "##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags ) )
	{
		if ( ImGui::BeginMenuBar() )
		{
			//drawBuildWindow();
			//m_runComboButton.draw();
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}

void wv::EditorInterfaceSystem::renderStatusBar()
{
	ImGuiViewportP* viewport = (ImGuiViewportP*)ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
	float height = ImGui::GetFrameHeight();

	if ( ImGui::BeginViewportSideBar( "##MainStatusBar", viewport, ImGuiDir_Down, height, window_flags ) )
	{
		if ( ImGui::BeginMenuBar() )
		{
			ImGui::Text( "FPS: %f", m_averageFPS );
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}