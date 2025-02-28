#pragma once

#include <wv/editor/ieditor_window.h>

namespace wv {

class TargetManagerWindow : public IEditorWindow
{
public:
	TargetManagerWindow() : IEditorWindow( "Target Manager" ) {}

	void draw() override;

};

}