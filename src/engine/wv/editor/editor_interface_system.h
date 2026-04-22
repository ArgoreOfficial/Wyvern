#pragma once

#include <wv/entity/ecs.h>

namespace wv {

struct EditorObjectComponent
{

};

class EditorInterfaceSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override {
		m_debugName = "EditorInterfaceSystem";

		setUpdateMode( UpdateMode_always );

		_config.addComponentType<EditorObjectComponent>();
	}

	virtual void onInitialize() override;
	virtual void onUpdate() override;
	virtual void onEditorRender() override;

private:
	void renderPrimaryMenuBar();
	void renderSecondaryMenuBar();
	void renderStatusBar();

	void reloadMaterials();

	void renderEntityView();
	void renderMaterialView();

	Entity* m_editorCameraEntity  = nullptr;
	
	bool m_showSystemsMenu = false;
	bool m_showMaterialMenu = true;
	bool m_showEntitiesMenu = true;

	bool m_hasEnabledFirstFrame = false;

	double m_timeSinceFPSUpdate = 0.0;
	size_t m_framesSinceFPSUpdate = 0;
	double m_averageFPS = 0.0;

	int m_currentSelectedEntity = 0;
	int m_currentMaterialViewSelected = 0;
	int m_currentMaterialViewTextureSelected = 0;
};

}