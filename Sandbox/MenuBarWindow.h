#pragma once

#include <Wyvern/Core/iSceneObject.h>

class MenuBarWindow : public wv::iSceneObject
{
public:
	MenuBarWindow() { }
	~MenuBarWindow() { }

	void drawUI() override;

private:

};