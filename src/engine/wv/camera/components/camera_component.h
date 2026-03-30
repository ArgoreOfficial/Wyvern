#pragma once

namespace wv {

class ViewVolume;

struct CameraComponent
{
	ViewVolume* viewVolume{ nullptr };
	bool active = false;
};

}