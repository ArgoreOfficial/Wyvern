#pragma once

#include <wv/entity/world.h>

class GameWorld : public wv::World
{
public:
	GameWorld() = default;
	virtual ~GameWorld() { }

protected:
	virtual void onSetupInput( wv::InputSystem* _inputSystem ) override;
	virtual void onSceneCreate() override;
};