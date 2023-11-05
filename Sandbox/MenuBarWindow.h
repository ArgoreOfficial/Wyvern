#pragma once

#include <Wyvern/Core/ISceneObject.h>

class MenuBarWindow : public WV::ISceneObject
{
public:
	MenuBarWindow() { }
	~MenuBarWindow() { }

	void drawUI() override;

private:

};