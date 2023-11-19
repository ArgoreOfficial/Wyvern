#pragma once

#include <Wyvern/Core/iSceneObject.h>

class SceneGraphWindow : public WV::iSceneObject
{
public:
	SceneGraphWindow() { }
	~SceneGraphWindow() { }

	void drawUI() override;

private:

};