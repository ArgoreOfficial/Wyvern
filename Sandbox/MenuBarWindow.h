#pragma once

#include <Wyvern/Core/iSceneObject.h>

class MenuBarWindow : public WV::iSceneObject
{
public:
	MenuBarWindow() { }
	~MenuBarWindow() { }

	void drawUI() override;

private:

};