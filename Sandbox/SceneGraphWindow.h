#pragma once

#include <Wyvern/Core/ISceneObject.h>

class SceneGraphWindow : public WV::ISceneObject
{
public:
	SceneGraphWindow() { }
	~SceneGraphWindow() { }

	void drawUI() override;

private:

};