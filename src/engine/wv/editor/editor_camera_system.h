#pragma once

#include <wv/entity/ecs.h>

#include <wv/input/action_group.h>

namespace wv {

struct EditorCameraComponent
{
	bool active = false;
	bool isMoving = false;

	Vector2f look{ 0.0f, 0.0f };
	Vector2f move{ 0.0f, 0.0f };

	static inline wv::Reflection reflection{}; // empty reflection
};

class EditorCameraSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override;

	virtual void onInitialize() override;
	virtual void onUpdate() override;

	bool isCameraLocked() const { return m_isCameraLocked; }

private:
	wv::Vector2f m_moveInput = {};

	wv::ActionGroup* m_editorActionGroup = nullptr;
	wv::ActionID m_moveActionID = {};
	
	bool m_isCameraLocked = false;

	float m_moveSpeed = 50.0f;
};

}