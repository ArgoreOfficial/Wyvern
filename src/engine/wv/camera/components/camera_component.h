#pragma once

namespace wv {

class ViewVolume;

struct CameraComponent
{
	ViewVolume* m_viewVolume{ nullptr };
};

}