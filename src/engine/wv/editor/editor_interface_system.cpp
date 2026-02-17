#include "editor_interface_system.h"

#include <wv/application.h>
#include <wv/entity/entity.h>
#include <wv/entity/world.h>

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
			_selection->selectedEntity = nullptr;
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
		_selection->selectedEntity = _entity;
		_selection->selectedComponent = nullptr;
	}

	if ( isOpen )
	{

		for ( auto comp : components )
			componentTreeNode( comp, _selection );
		
		ImGui::TreePop();
	}
}

void worldSystemTreeNode( wv::IWorldSystem* _sys )
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_Leaf;

	std::string label = std::format( "{}", _sys->getTypeName() );

	if ( ImGui::TreeNodeEx( label.c_str(), flags ) )
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
				worldSystemTreeNode( worldSystem );
			
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

		ImGui::SeparatorText( "Properties" );

		if ( ImGui::BeginChild( "###SelectedProperties", ImVec2( -FLT_MIN, wsize.y / 4 ) ) )
		{
			if ( m_selection.selectedEntity )
			{
				Entity* ent = m_selection.selectedEntity;
				ImGui::Text( "Entity" );
				ImGui::Text( "Name: %s", ent->getName().c_str());
				ImGui::Text( "UUID: %s", ent->getID().toString().c_str());
			}
			else if ( m_selection.selectedComponent )
			{
				IEntityComponent* comp = m_selection.selectedComponent;
				ImGui::Text( "Component" );
				ImGui::Text( "Type: %s", comp->getTypeName().c_str() );
				ImGui::Text( "UUID: %s", comp->getID().toString().c_str());

			}

			ImGui::EndChild();
		}

		ImGui::End();
	}

}
