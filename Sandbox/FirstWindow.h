#pragma once

#include <Wyvern/Core/iSceneObject.h>

class FirstWindow : public wv::iSceneObject
{
public:
	FirstWindow();
	~FirstWindow();

	void drawUI() override;

private:

};