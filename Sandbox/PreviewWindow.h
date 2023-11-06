#pragma once

#include <Wyvern/Core/ISceneObject.h>

class PreviewWindow : public WV::ISceneObject
{
public:
	PreviewWindow() { }
	~PreviewWindow() { }

	void drawUI() override;

private:

};