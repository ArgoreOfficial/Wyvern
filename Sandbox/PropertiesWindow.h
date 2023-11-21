#pragma once

#include <Wyvern/Core/iSceneObject.h>

class PropertiesWindow : public wv::iSceneObject
{
public:
	PropertiesWindow();
	~PropertiesWindow();

	void drawUI() override;

private:

};