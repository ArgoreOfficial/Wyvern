#pragma once

#include <Wyvern/Core/iSceneObject.h>

class PropertiesWindow : public WV::iSceneObject
{
public:
	PropertiesWindow();
	~PropertiesWindow();

	void drawUI() override;

private:

};