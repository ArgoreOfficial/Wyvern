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
	void updateEditorState();

	void renderPrimaryMenuBar();
	void renderSecondaryMenuBar();
	void renderStatusBar();

	Entity* m_editorCameraEntity  = nullptr;
	Entity* m_runtimeCameraEntity = nullptr;

	bool m_showSystemsMenu = false;

	double m_timeSinceFPSUpdate = 0.0;
	size_t m_framesSinceFPSUpdate = 0;
	double m_averageFPS = 0.0;
};

}