#include "editor_interface_system.h"

#include <wv/entity/entity.h>

#include <imgui.h>

void wv::EditorInterfaceSystem::onDebugRender()
{
	auto entities = m_components.getEntities();
	Entity* selectedEntity = nullptr;

	if ( ImGui::Begin( "Scene View" ) )
	{
		for ( size_t i = 0; i < entities.size(); i++ )
		{
			Entity* ent = entities[ i ];

			ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen;
			flag |= ImGuiTreeNodeFlags_Leaf;

			if ( ent->getID() == m_selectedUUID )
			{
				selectedEntity = ent;
				flag |= ImGuiTreeNodeFlags_Selected;
			}

			if ( ImGui::TreeNodeEx( ent->getID().toString().c_str(), flag) )
			{
				if ( ImGui::IsItemClicked() )
					m_selectedUUID = ent->getID();
				
				// Call ImGui::TreeNodeEx() recursively to populate each level of children
				ImGui::TreePop(); 
			}
		}

		ImGui::End();
	}

	if ( selectedEntity )
	{
		if( ImGui::Begin( "Entity Properties" ) )
		{
			for ( auto component : selectedEntity->getComponents() )
			{
				ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_DefaultOpen;
				flag |= ImGuiTreeNodeFlags_Leaf;

				if ( ImGui::TreeNodeEx( component->getTypeName().c_str(), flag) )
				{
					// Call ImGui::TreeNodeEx() recursively to populate each level of children
					ImGui::TreePop();
				}
			}

			ImGui::End();
		}
	}

}
