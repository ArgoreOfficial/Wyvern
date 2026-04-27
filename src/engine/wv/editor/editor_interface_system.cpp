#include "editor_interface_system.h"

#include <wv/application.h>
#include <wv/entity/world.h>
#include <wv/input/input_system.h>

#include <wv/components/camera_component.h>
#include <wv/systems/camera_manager_system.h>
#include <wv/editor/editor_camera_system.h>
#include <wv/editor/mesh_importer_gltf.h>

#include <wv/rendering/material.h>
#include <wv/rendering/texture.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <set>

void wv::EditorInterfaceSystem::onInitialize()
{
	setEditorRenderEnabled( true );

	m_editorActionGroup  = updateContext->inputSystem->getActionGroup( "Editor" );
	
	m_mouseLeftActionID  = m_editorActionGroup->getTriggerActionID( "MouseLeft" );
	m_mouseRightActionID = m_editorActionGroup->getTriggerActionID( "MouseRight" );

	m_shiftActionID = m_editorActionGroup->getValueActionID( "ShiftLeft" );

	World* world = getWorld();

	m_editorCameraEntity = world->createEntity( "EditorCamera" );
	m_editorCameraEntity->setShouldSerialize( false );
	m_editorCameraEntity->setIsPersistent( true );

	world->addComponent<wv::CameraComponent>( m_editorCameraEntity, { .active = false } );
	world->addComponent<wv::EditorCameraComponent>( m_editorCameraEntity, {} );
	
	m_editorCameraEntity->getTransform().setPosition( { 0.0f, 10.0f, 10.0f } );

}

void wv::EditorInterfaceSystem::onUpdate()
{
	InputSystem* inputSystem = getApp()->getInputSystem();
	World* world = getWorld();
	
	for ( ActionEvent& ae : updateContext->actionEventQueue )
	{
		if ( ae.actionID == m_shiftActionID ) 
			m_leftShiftState = ae.action.value->getValue() > 0.001f;
	}

	if ( !m_hasEnabledFirstFrame )
	{
		m_editorActionGroup->enable();
		world->toggleEditorState();
		m_hasEnabledFirstFrame = true;
	}

	CameraManagerSystem* cameraManagerSystem = world->getSystem<CameraManagerSystem>();

	bool editorState = world->isEditorState();
	auto& comp = world->getComponent<EditorCameraComponent>( m_editorCameraEntity );
	comp.active = editorState;
	cameraManagerSystem->setCameraOverride( editorState ? m_editorCameraEntity : nullptr );

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

	for ( Archetype* arch : getArchetypes() )
	{
		auto& comps = arch->getComponents<EditorObjectComponent>();
		auto& entities = arch->getEntities();

		// update selected entities

		for ( size_t i = 0; i < arch->getNumEntities(); i++ )
			comps[ i ].selected = m_selectedEntities.contains( entities[ i ]->getID() );
		
	}
}

void wv::EditorInterfaceSystem::onEditorRender()
{
#ifdef WV_SUPPORT_IMGUI
	renderPrimaryMenuBar();
	renderSecondaryMenuBar();
	renderStatusBar();

	if ( m_showSystemsMenu )
	{
		if ( ImGui::Begin( "Systems" ) )
		{
			auto systems = getWorld()->getSystems();
			for ( ISystem* system : systems )
			{
				std::string systemName = system->getDebugName();

				if ( systemName == "" )
					continue;
				
				// we don't wanna accidentally turn off ourselves lmfao
				if ( systemName == m_debugName )
					continue; 

				bool editorEnabled = system->getEditorRenderEnabled();

				ImGui::Text( systemName.c_str() );
				std::string editorToolLabel = "Enable Editor Tools##" + systemName;
				if ( ImGui::Checkbox( editorToolLabel.c_str(), &editorEnabled) )
					system->setEditorRenderEnabled( editorEnabled );
			}
		}
		ImGui::End();
	}

	if ( m_showMaterialMenu )
		renderMaterialView();

	if ( m_showEntitiesMenu )
	{
		renderEntityView();
		renderComponentView();
	}

	if ( m_showMeshImporter )
	{
		if ( ImGui::Begin( "Meshes##mesh_importer_window" ) )
		{
			ImGui::InputTextWithHint( "Path", "meshes/mesh.glb", m_meshImporterPath, 1024 );

			if ( ImGui::Button( "Import##mesh_import_button" ) )
			{
				MeshImporterGLTF importer{};
				std::string str = m_meshImporterPath;
				importer.load( str );
			}

			ImGui::SameLine();

			if ( ImGui::Button( "Import & Create##mesh_import_create_button" ) )
			{
				MeshImporterGLTF importer{};
				std::string str = m_meshImporterPath;
				importer.load( str );

				if ( importer.hasLoaded() )
					importer.createEntity( getWorld(), "New Mesh" );
			}
		}
		ImGui::End();
	}
#endif
}

void wv::EditorInterfaceSystem::renderPrimaryMenuBar()
{
	World* world = getWorld();

	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( "File" ) )
		{
			if ( ImGui::MenuItem( "Save" ) )
				world->save( "worlds/test_world.world" );
			
			if ( ImGui::MenuItem( "Reload" ) ) 
				world->reload( false );
			if ( ImGui::IsItemHovered() ) 
				ImGui::SetTooltip( "Reloads only scene" );
			
			if ( ImGui::MenuItem( "Reload All" ) ) 
				world->reload( true );
			if ( ImGui::IsItemHovered() ) 
				ImGui::SetTooltip( "Reloads scene and assets" );

			//
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Tools" ) )
		{
			if ( ImGui::MenuItem( "Reload Materials" ) ) reloadMaterials();
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

	World* world = getWorld();

	if ( ImGui::BeginViewportSideBar( "##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags ) )
	{
		if ( ImGui::BeginMenuBar() )
		{
			if ( ImGui::ArrowButton( "Runtime", ImGuiDir_Right ) )
			{
				world->toggleEditorState();
				if ( world->isEditorState() )
				{
					m_editorActionGroup->enable();
					world->reload( false );
				}
				else
				{
					m_editorActionGroup->disable();
					world->save( world->getPath() );
					world->reload( false );
				}
			}
			
			if ( ImGui::Button( "Entities" ) )
				m_showEntitiesMenu = !m_showEntitiesMenu;

			if ( ImGui::Button( "Systems" ) )
				m_showSystemsMenu = !m_showSystemsMenu;
			
			if ( ImGui::Button( "Materials" ) )
				m_showMaterialMenu = !m_showMaterialMenu;
			
			if ( ImGui::Button( "Meshes" ) )
				m_showMeshImporter = !m_showMeshImporter;

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

void wv::EditorInterfaceSystem::reloadMaterials()
{
	auto materials = wv::getApp()->getMaterialManager()->getManaged();

	for ( Ref<MaterialAsset> mat : materials )
		mat->reload();
}

void wv::EditorInterfaceSystem::renderEntityTreeNode( Entity* _entity )
{
	std::string entityNodeID = _entity->getName() + "##" + std::to_string( (uint64_t)_entity->getID() );

	ImGuiTreeNodeFlags flags 
		= ImGuiTreeNodeFlags_DefaultOpen 
		| ImGuiTreeNodeFlags_OpenOnArrow 
		| ImGuiTreeNodeFlags_OpenOnDoubleClick 
		| ImGuiTreeNodeFlags_DrawLinesToNodes;

	auto children = _entity->getChildren();

	if ( children.empty() )
		flags |= ImGuiTreeNodeFlags_Leaf;

	UUID id = _entity->getID();
	
	if ( m_selectedEntities.contains( id ) )
		flags |= ImGuiTreeNodeFlags_Selected;

	if ( ImGui::TreeNodeEx( entityNodeID.c_str(), flags ) )
	{
		if ( ImGui::IsItemClicked() )
		{
			if( !m_leftShiftState )
				m_selectedEntities.clear();

			if ( m_selectedEntities.contains( id ) )
				m_selectedEntities.erase( id );
			else
				m_selectedEntities.insert( id );
		}

		for ( Entity* e : children )
			renderEntityTreeNode( e );

		ImGui::TreePop();
	}
}

void wv::EditorInterfaceSystem::renderEntityView()
{
	if ( ImGui::Begin( "Scene View##editor_scene_window" ) )
	{
		std::vector<Entity*> entities = getWorld()->getActiveEntities();
		
		if ( ImGui::TreeNodeEx( "World##world_root_node", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			for ( Entity* e : entities )
			{
				if ( e->getParent() )
					continue;

				renderEntityTreeNode( e );
			}
			ImGui::TreePop();
		}

	}
	ImGui::End();
}

void wv::EditorInterfaceSystem::renderComponentView()
{
	World* world = getWorld();

	int editorObjectIndex = ECSEngine::ComponentTypeDef<EditorObjectComponent>::index;
	int editorCameraIndex = ECSEngine::ComponentTypeDef<EditorCameraComponent>::index;
	bool destroy = false;

	Entity* selectedEntity = nullptr;

	if ( m_selectedEntities.size() > 0 )
		selectedEntity = world->getEntityFromID( *m_selectedEntities.begin() );
	
	if( !selectedEntity )
		return;

	if ( ImGui::Begin( "Entity Properties##entity_properties_window" ) )
	{
		ImGui::SeparatorText( selectedEntity->getName().c_str() );
		std::vector<int> allComponent = world->getRegisteredComponents();
		std::set<int> existingComponents;

		if ( selectedEntity->archetype )
		{
			std::vector<int> componentIndices = selectedEntity->archetype->getComponentIndices();
			for ( int& index : componentIndices )
				existingComponents.insert( index );
		}
			
		if ( ImGui::BeginMenu( "Actions##entity_menu_actions" ) )
		{
			if ( ImGui::BeginMenu( "Add Component" ) )
			{
				for ( int index : allComponent )
				{
					if ( index == editorObjectIndex || index == editorCameraIndex )
						continue;

					std::string compName = world->getComponentName( index );
					bool hasComponent = existingComponents.contains( index );

					if ( hasComponent )
						ImGui::BeginDisabled();

					if ( ImGui::Selectable( compName.c_str() ) )
						world->addComponent( index, selectedEntity );

					if ( hasComponent )
						ImGui::EndDisabled();
				}
				ImGui::EndMenu();
			}

			if ( ImGui::Selectable( "Delete##entity_destroy_selectable" ) )
				destroy = true;

			ImGui::EndMenu();
		}

		WorldSerializer* serializer = world->getWorldSerializer();

		if ( selectedEntity->archetype )
		{
			std::vector<int> componentIndices = selectedEntity->archetype->getComponentIndices();
				
			for ( int& index : componentIndices )
			{
				if ( index == editorObjectIndex || index == editorCameraIndex )
					continue;

				std::string componentNameID = world->getComponentName( index ) + "##" + std::to_string( selectedEntity->getID() );
				if ( ImGui::CollapsingHeader( componentNameID.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					ImGui::PushID( index );
					if ( ImGui::Button( "Delete##component_delete_button" ) )
						world->removeComponent( index, selectedEntity );
					ImGui::PopID();

					if ( serializer->hasSerializeInfo( index ) )
					{
						const SerializeInfo& info = serializer->getSerializeInfo( index );
						for ( auto& m : info.members )
						{
							ImGui::Text( "%s value: FIXME", m->name.c_str() );
						}
					}
				}
			}
		}
	}
	ImGui::End();

	if ( destroy )
	{
		m_selectedEntities.erase( selectedEntity->getID() );
		getWorld()->removeAllComponents( selectedEntity );
		getWorld()->destroyEntity( selectedEntity );
	}

}

void wv::EditorInterfaceSystem::renderMaterialView()
{
	auto materials = wv::getApp()->getMaterialManager()->getManaged();
	std::vector<std::string> materialNames;
	materialNames.reserve( materials.size() );

	std::vector<const char*> materialNamesCstr;

	size_t i = 0;
	for ( Ref<MaterialAsset> mat : materials )
	{
		materialNames.push_back( mat->path.string() );
		materialNamesCstr.push_back( materialNames.back().c_str() );
	}
	
	if ( ImGui::Begin( "Material View" ) )
	{
		ImGui::ListBox( "Materials", &m_currentMaterialViewSelected, materialNamesCstr.data(), materialNamesCstr.size() );

		auto& textureAssets = materials[ m_currentMaterialViewSelected ]->textureAssets;
		std::vector<std::string> textureNames;
		textureNames.reserve( textureAssets.size() );

		std::vector<const char*> textureNamesCstr;

		for ( Ref<TextureAsset> tex : textureAssets )
		{
			textureNames.push_back( tex->getPath().string() );
			textureNamesCstr.push_back( textureNames.back().c_str() );
		}

		ImGui::ListBox( "Textures", &m_currentMaterialViewTextureSelected, textureNamesCstr.data(), textureNamesCstr.size() );

	}



	ImGui::End();
}
