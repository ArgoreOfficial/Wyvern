#pragma once

#include <Wyvern/Core/iSceneObject.h>

class PreviewWindow : public wv::iSceneObject
{
public:
	PreviewWindow() { }
	~PreviewWindow() { }

	void drawUI() override;

private:

};