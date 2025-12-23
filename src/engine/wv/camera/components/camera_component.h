#pragma once

#include <wv/entity/entity_component.h>

namespace wv {

class ICamera;

class CameraComponent : public IEntityComponent
{
	friend class CameraManagerSystem;

	WV_REFLECT_TYPE( CameraComponent )
public:
	CameraComponent() = default;
	virtual ~CameraComponent() { }

	ICamera* getUnderlyingCamera() { return m_camera; }

protected:

	ICamera* m_camera;

};

}