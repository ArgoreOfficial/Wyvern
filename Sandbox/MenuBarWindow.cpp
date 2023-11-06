#include "MenuBarWindow.h"
#include <imgui.h>

void MenuBarWindow::drawUI()
{
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
}
