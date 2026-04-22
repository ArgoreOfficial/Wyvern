#include "editor_interface_system.h"

#include <wv/application.h>
#include <wv/entity/world.h>
#include <wv/input/input_system.h>

#include <wv/components/camera_component.h>
#include <wv/systems/camera_manager_system.h>
#include <wv/editor/editor_camera_system.h>

#include <wv/rendering/material.h>
#include <wv/rendering/texture.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

void wv::EditorInterfaceSystem::onInitialize()
{
	setEditorRenderEnabled( true );

	m_editorActionGroup  = updateContext->inputSystem->getActionGroup( "Editor" );
	m_moveObjectActionID = m_editorActionGroup->getTriggerActionID( "MoveObject" );
	m_mouseLeftActionID  = m_editorActionGroup->getTriggerActionID( "MouseLeft" );
	m_mouseRightActionID = m_editorActionGroup->getTriggerActionID( "MouseRight" );

	World* world = getApp()->getWorld();

	m_editorCameraEntity = world->createEntity( "EditorCamera" );
	m_editorCameraEntity->setShouldSerialize( false );
	m_editorCameraEntity->setIsPersistent( true );

	world->addComponent<wv::CameraComponent>( m_editorCameraEntity, { .active = false } );
	world->addComponent<wv::EditorCameraComponent>( m_editorCameraEntity, {} );
	
	m_editorCameraEntity->getTransform().setPosition( { 0.0f, 10.0f, 10.0f } );

}

void wv::EditorInterfaceSystem::onUpdate()
{
	Application* app = getApp();
	InputSystem* inputSystem = app->getInputSystem();
	World* world = app->getWorld();
	
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

	for ( ActionEvent& ae : updateContext->actionEventQueue )
	{
		if ( ae.actionID == m_moveObjectActionID && ae.action.trigger->getValue() )
		{
			if( m_isMovingObject )
				endMoveObject( true );
			else
				beginMoveObject();
		}
		
		if ( ae.actionID == m_mouseLeftActionID )
			if ( m_isMovingObject ) endMoveObject( true );
		
		if ( ae.actionID == m_mouseRightActionID )
			if ( m_isMovingObject ) endMoveObject( false );
	}

	if ( m_selectedEntity && m_isMovingObject )
	{
		Transform& tfm = m_selectedEntity->getTransform();
		float dist = ( tfm.position - m_editorCameraEntity->getTransform().position ).length();

		Vector2f mouseMove = (Vector2f)inputSystem->getMouseMotion() / 1000.0f * dist;
		
		Vector3f right = m_editorCameraEntity->getTransform().right();
		Vector3f up = m_editorCameraEntity->getTransform().up();

		tfm.position += right * mouseMove.x + up * -mouseMove.y;
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
			auto systems = wv::getApp()->getWorld()->getSystems();
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
		renderEntityView();
#endif
}

void wv::EditorInterfaceSystem::beginMoveObject()
{
	if ( m_isMovingObject )
		return;

	Application* app = getApp();
	m_isMovingObject = true;
	
	app->setCursorLock( true );
	m_moveObjectStartPosition = m_selectedEntity->getTransform().position;
}

void wv::EditorInterfaceSystem::endMoveObject( bool _confirm )
{
	if ( !m_isMovingObject )
		return;

	Application* app = getApp();
	m_isMovingObject = false;

	app->setCursorLock( false );

	if( !_confirm )
		m_selectedEntity->getTransform().position = m_moveObjectStartPosition;
}

void wv::EditorInterfaceSystem::renderPrimaryMenuBar()
{
	World* world = getApp()->getWorld();

	if ( ImGui::BeginMainMenuBar() )
	{
		if ( ImGui::BeginMenu( "File" ) )
		{
			if ( ImGui::MenuItem( "Save" ) )
				world->save( "worlds/test_world.world" );
			
			if ( ImGui::MenuItem( "Reload" ) )
				world->reload();
			
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

	World* world = getApp()->getWorld();

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
					world->reload();
				}
				else
					m_editorActionGroup->disable();
			}
			
			if ( ImGui::Button( "Entities" ) )
				m_showEntitiesMenu = !m_showEntitiesMenu;

			if ( ImGui::Button( "Systems" ) )
				m_showSystemsMenu = !m_showSystemsMenu;
			
			if ( ImGui::Button( "Materials" ) )
				m_showMaterialMenu = !m_showMaterialMenu;
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

void wv::EditorInterfaceSystem::renderEntityView()
{
	if( ImGui::Begin( "Entities" ) )
	{
		auto entities = wv::getApp()->getWorld()->getActiveEntities();
		std::vector<std::string> entityNames;
		std::vector<const char*> entityNamesCstr;

		entityNames.reserve( entities.size() );

		for ( Entity* e : entities )
		{
			entityNames.push_back( e->getName() );
			entityNamesCstr.push_back( entityNames.back().c_str());
		}
		
		ImGui::ListBox( "Entities", &m_currentSelectedEntityIndex, entityNamesCstr.data(), entityNamesCstr.size() );

		if ( m_currentSelectedEntityIndex >= 0 && m_currentSelectedEntityIndex < entities.size() )
			m_selectedEntity = entities[ m_currentSelectedEntityIndex ];
		else
			m_selectedEntity = nullptr;
	}
	ImGui::End();
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
