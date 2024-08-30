#include "SceneGraphWindow.h"

#include <wv/Engine/Engine.h>
#include <wv/Engine/ApplicationState.h>

#include <wv/Scene/SceneRoot.h>

#include <imgui.h>

void wv::Editor::cSceneGraphWindow::draw( wv::iGraphicsDevice* _pGraphicsDevice )
{
	wv::cEngine* engine = cEngine::get();
	cSceneRoot* sceneRoot = engine->m_pApplicationState->getCurrentScene();

	if( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
		m_selectedObjects.clear();

	ImGui::Begin( "SceneGraph" );
	
	if( sceneRoot )
		for( auto& child : sceneRoot->getChildren() )
			drawSceneObject( child );
	
	ImGui::End();
}

void wv::Editor::cSceneGraphWindow::drawSceneObject( wv::iSceneObject* _obj )
{
	if( !_obj )
		return;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
	if( _obj->getChildren().empty() )
		flags |= ImGuiTreeNodeFlags_Leaf;

	for( auto& selected : m_selectedObjects )
	{
		if( selected != _obj )
			continue;

		flags |= ImGuiTreeNodeFlags_Selected;
		break;
	}

	bool opened = ImGui::TreeNodeEx( _obj, flags, _obj->getName().c_str() );
	if( ImGui::IsItemClicked() )
	{
		// if ( !ctrl clicking )
			m_selectedObjects.clear();
		
		m_selectedObjects.push_back( _obj );
	}

	if( opened )
	{
		for( auto& child : _obj->getChildren() )
			drawSceneObject( child );
		ImGui::TreePop();
	}
	
}