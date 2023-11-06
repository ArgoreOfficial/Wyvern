#pragma once

#include <Wyvern/Core/ISceneObject.h>

class PropertiesWindow : public WV::ISceneObject
{
public:
	PropertiesWindow();
	~PropertiesWindow();

	void drawUI() override;

private:

};