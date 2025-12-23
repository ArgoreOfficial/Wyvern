#pragma once

#include <wv/entity/entity_component.h>

namespace wv {

class ViewVolume;

class CameraComponent : public IEntityComponent
{
	friend class CameraManagerSystem;

	WV_REFLECT_TYPE( CameraComponent )
public:
	CameraComponent() = default;
	virtual ~CameraComponent() { }

	ViewVolume* getUnderlyingCamera() { return m_camera; }

protected:

	ViewVolume* m_camera;

};

}