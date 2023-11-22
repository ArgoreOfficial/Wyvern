#pragma once

#include <Wyvern/Core/iSceneObject.h>

class SecondWindow : public wv::iSceneObject
{
public:
	SecondWindow() { }
	~SecondWindow() { }

	void drawUI() override;

private:

};