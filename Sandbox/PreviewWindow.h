#pragma once

#include <Wyvern/Core/iSceneObject.h>

class PreviewWindow : public WV::iSceneObject
{
public:
	PreviewWindow() { }
	~PreviewWindow() { }

	void drawUI() override;

private:

};