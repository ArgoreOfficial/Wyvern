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
	void setIsMoveObject( bool _moving );

	ActionGroup* m_editorActionGroup = nullptr;
	
	ActionID m_moveObjectActionID = {};
	ActionID m_moveObjectXActionID = {};
	ActionID m_moveObjectYActionID = {};
	ActionID m_moveObjectZActionID = {};

	ActionID m_shiftActionID = {};
	ActionID m_mouseLeftActionID = {};
	ActionID m_mouseRightActionID = {};

	bool m_isMovingObject = false;
	int m_lockMovementAxis = -1;

	bool m_leftShiftState = false;

	Vector2f m_accumulatedMouseMove;

};

}