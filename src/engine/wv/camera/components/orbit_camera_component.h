#include "camera_component.h"

namespace wv {

class OrbitCameraComponent : public CameraComponent
{
	WV_REFLECT_TYPE( OrbitCameraComponent, CameraComponent )
public:
	OrbitCameraComponent();
	~OrbitCameraComponent();

protected:

};

}