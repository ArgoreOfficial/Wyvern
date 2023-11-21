#pragma once

#include <Wyvern/Core/iSceneObject.h>

class SceneGraphWindow : public wv::iSceneObject
{
public:
	SceneGraphWindow() { }
	~SceneGraphWindow() { }

	void drawUI() override;

private:

};