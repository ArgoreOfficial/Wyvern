#pragma once

#include <wv/camera/view_volume.h>

namespace wv {

class ViewVolume;

struct CameraComponent
{
	ViewVolume viewVolume{};
	bool active = false;
};

}