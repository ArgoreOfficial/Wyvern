#include "FirstWindow.h"
#include <imgui.h>

FirstWindow::FirstWindow()
{
}

FirstWindow::~FirstWindow()
{
}

void FirstWindow::drawUI()
{
	ImGui::SetNextWindowViewport( ImGui::GetMainViewport()->ID ); // disable multi-viewport on this window
	if ( ImGui::Begin( "Properties" ) )
	{

	}
	ImGui::End();
}
