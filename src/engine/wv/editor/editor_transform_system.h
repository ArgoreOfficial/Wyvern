#pragma once

#include <wv/editor/editor_object_component.h>
#include <wv/entity/ecs.h>
#include <wv/input/action_group.h>

namespace wv {

class EditorTransformSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override {
		setDebugName( "EditorTransformSystem" );
		setUpdateMode( UpdateMode_always );

		_config.addComponentType<EditorObjectComponent>();
	}

	virtual void onInitialize() override;
	virtual void onUpdate() override;
	virtual void onEditorRender() override;

private:
	enum EditTransformMode
	{
		EditTransformMode_None,
		EditTransformMode_Translate,
		EditTransformMode_Rotate,
		EditTransformMode_Scale
	};

	void setEditTransformMode( EditTransformMode _mode );

	void translateObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength );
	void rotateObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength );
	void scaleObject( Entity* _entity, EditorObjectComponent& _editorComponent, Vector3f _com, float _strength );

	ActionGroup* m_editorActionGroup = nullptr;
	
	ActionID m_editTranslateActionID{};
	ActionID m_editRotateActionID{};
	ActionID m_editScaleActionID{};

	ActionID m_lockAxisXActionID{};
	ActionID m_lockAxisYActionID{};
	ActionID m_lockAxisZActionID{};

	ActionID m_shiftActionID = {};
	ActionID m_mouseLeftActionID = {};
	ActionID m_mouseRightActionID = {};

	EditTransformMode m_transformMode = EditTransformMode_None;
	int m_lockMovementAxis = -1;

	bool m_leftShiftState = false;

	Vector2f m_accumulatedMouseMove;

};

}