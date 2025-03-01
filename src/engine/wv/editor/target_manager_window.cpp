#include "target_manager_window.h"

#ifdef WV_SUPPORT_IMGUI
#include <imgui.h>
#endif

void wv::TargetManagerWindow::draw()
{
#ifdef WV_SUPPORT_IMGUI
	if ( ImGui::Begin( "Target Manager", 0, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		ImGui::Button( "Add Target" );
		ImGui::SameLine();
		ImGui::Button( "Remove Target" );
		
		const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
		static int listbox_item_current = 1;
		ImGui::ListBox( "listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE( listbox_items ), 4 );
	}
	ImGui::End();
#endif
}
