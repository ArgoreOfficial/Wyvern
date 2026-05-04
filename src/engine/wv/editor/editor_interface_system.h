#pragma once

#include <wv/editor/editor_object_component.h>
#include <wv/entity/ecs.h>
#include <wv/input/action_group.h>

#include <set>

namespace wv {

class EditorInterfaceSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override {
		setDebugName( "EditorInterfaceSystem" );
		setUpdateMode( UpdateMode_Always );

		_config.addComponentType<EditorObjectComponent>();
	}

	virtual void onInitialize() override;
	virtual void onUpdate() override;
	virtual void onEditorRender() override;

	Entity* getEditorCamera() const { return m_editorCameraEntity; }

private:
	void renderPrimaryMenuBar();
	void renderSecondaryMenuBar();
	void renderStatusBar();

	void reloadMaterials();

	void renderEntityTreeNode( Entity* _entity );
	void renderEntityView();
	void renderComponentView();
	void renderMaterialView();

	static constexpr const char* c_dragEntityTypeName = "WV_DND_ENTITY";

	Entity* m_editorCameraEntity  = nullptr;
	
	bool m_showSystemsMenu = false;
	bool m_showMaterialMenu = false;
	bool m_showEntitiesMenu = true;
	bool m_showMeshImporter = false;

	bool m_hasEnabledFirstFrame = false;

	double m_timeSinceFPSUpdate = 0.0;
	size_t m_framesSinceFPSUpdate = 0;
	double m_averageFPS = 0.0;

	wv::ActionGroup* m_editorActionGroup = nullptr;
	
	wv::ActionID m_shiftActionID = {};
	wv::ActionID m_mouseLeftActionID = {};
	wv::ActionID m_mouseRightActionID = {};

	bool m_leftShiftState = false;

	int m_currentSelectedEntityIndex = 0;
	int m_currentMaterialViewSelected = 0;
	int m_currentMaterialViewTextureSelected = 0;

	std::set<UUID> m_selectedEntities;

	char m_meshImporterPath[ 1024 ];

	std::pair<Entity*, Entity*> m_moveEntityHierarchyChange{ nullptr, nullptr };
};

}