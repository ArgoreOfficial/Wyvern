#include "editor_interface_system.h"

#include <wv/application.h>
#include <wv/entity/entity.h>
#include <wv/entity/world.h>

#include <wv/reflection/reflection.h>

#include <imgui.h>

void componentTreeNode( wv::IEntityComponent* _comp, wv::SceneViewSelection* _selection )
{
	ImGuiTreeNodeFlags compFlag = ImGuiTreeNodeFlags_DefaultOpen;
	compFlag |= ImGuiTreeNodeFlags_Leaf;

	if ( _comp == _selection->selectedComponent )
		compFlag |= ImGuiTreeNodeFlags_Selected;

	std::string compLabel = std::format( "{} ({})", _comp->getTypeName(), _comp->getID().toString() );

	if ( ImGui::TreeNodeEx( compLabel.c_str(), compFlag ) )
	{
		if ( ImGui::IsItemClicked() )
		{
			*_selection = {};

			_selection->selectedComponent = _comp;
		}

		ImGui::TreePop();
	}
}

void entityTreeNode( wv::Entity* _entity, wv::SceneViewSelection* _selection )
{
	auto components = _entity->getComponents();

	ImGuiTreeNodeFlags flags = 0; // ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_OpenOnArrow;

	if ( components.empty() )
		flags |= ImGuiTreeNodeFlags_Leaf;

	if ( _entity == _selection->selectedEntity )
		flags |= ImGuiTreeNodeFlags_Selected;
	
	std::string entLabel = std::format( "{} ({})", _entity->getName(), _entity->getID().toString() );

	bool isOpen = ImGui::TreeNodeEx( entLabel.c_str(), flags );
	
	if ( ImGui::IsItemClicked() )
	{
		*_selection = {};
		_selection->selectedEntity = _entity;
	}

	if ( isOpen )
	{

		for ( auto comp : components )
			componentTreeNode( comp, _selection );
		
		ImGui::TreePop();
	}
}

void worldSystemTreeNode( wv::IWorldSystem* _sys, wv::SceneViewSelection* _selection )
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_Leaf;

	if ( _sys == _selection->selectedWorldSystem )
		flags |= ImGuiTreeNodeFlags_Selected;

	std::string label = std::format( "{}", _sys->getTypeName() );

	bool isOpen = ImGui::TreeNodeEx( label.c_str(), flags );

	if ( ImGui::IsItemClicked() )
	{
		*_selection = {};
		_selection->selectedWorldSystem = _sys;
	}

	if( isOpen )
	{
		ImGui::TreePop();
	}
}

void wv::EditorInterfaceSystem::onDebugRender()
{
	auto entities = m_components.getEntities();
	Entity* selectedEntity = nullptr;
	IEntityComponent* selectedComponent = nullptr;

	if ( ImGui::Begin( "World" ) )
	{
		ImGui::SeparatorText( "World Systems" );

		Application* app = Application::getSingleton();
		World* world = app->getWorld();
		auto worldSystems = world->getWorldSystems();

		ImVec2 wsize = ImGui::GetWindowSize();
		
		if ( ImGui::BeginChild( "###WorldSystemTree", ImVec2( -FLT_MIN, wsize.y / 4 ) ) )
		{
			for ( IWorldSystem* worldSystem : worldSystems )
				worldSystemTreeNode( worldSystem, &m_selection );
			
			ImGui::EndChild();
		}

		ImGui::SeparatorText( "Entities" );

		if ( ImGui::BeginChild( "###SceneViewTree", ImVec2( -FLT_MIN, wsize.y / 4 ) ) )
		{
			for ( size_t i = 0; i < entities.size(); i++ )
			{
				Entity* ent = entities[ i ];
				entityTreeNode( ent, &m_selection );
			}
			ImGui::EndChild();
		}

		if ( ImGui::BeginChild( "###SelectedProperties", ImVec2( -FLT_MIN, wsize.y / 4 ) ) )
		{
			wv::IReflectedType* reflt = nullptr;

			if ( m_selection.selectedEntity )
			{
				Entity* ent = m_selection.selectedEntity;
				ImGui::SeparatorText( "Entity" );
				reflt = (wv::IReflectedType*)ent;
			}
			else if ( m_selection.selectedComponent )
			{
				IEntityComponent* comp = m_selection.selectedComponent;
				ImGui::SeparatorText( comp->getTypeName().c_str() );
				reflt = (wv::IReflectedType*)comp;
			}
			else if ( m_selection.selectedWorldSystem )
			{
				IWorldSystem* sys = m_selection.selectedWorldSystem;
				ImGui::SeparatorText( sys->getTypeName().c_str() );
				reflt = (wv::IReflectedType*)sys;
			}

			// Display reflected data

			if ( reflt )
			{
				auto inherited = reflt->getInheritedUUIDs();

				// Iterate backwards, so that inherited members appear before 

				for ( auto it = inherited.rbegin(); it != inherited.rend(); it++ )
				{
					wv::TypeInfo* typeInfo = wv::reflreg()->getTypeInfo( *it );
					if ( !typeInfo )
						continue;

					for ( auto* member : typeInfo->members )
					{
						std::string memberValue = std::format( "{}: {}", member->displayName, member->format( reflt ) );
						ImGui::Text( "%s", memberValue.c_str() );
					}
				}

				// Iterate final members

				{
					wv::TypeUUID typeUUID  = reflt->getTypeUUID();
					wv::TypeInfo* typeInfo = wv::reflreg()->getTypeInfo( typeUUID );

					for ( auto* member : typeInfo->members )
					{
						std::string memberValue = std::format( "{}: {}", member->displayName, member->format( reflt ) );
						ImGui::Text( "%s", memberValue.c_str() );
					}
				}
			}
			else
			{
				// if nothing is selected, just do a basic separator
				ImGui::SeparatorText("");
			}

			ImGui::EndChild();
		}

		ImGui::End();
	}

}
