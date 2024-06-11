#pragma once


#include "Ship.h"

#include <wv/Events/IMouseListener.h>
#include <wv/Events/IInputListener.h>

namespace wv { class ICamera; }

class PlayerShip : public Ship, public wv::IMouseListener, public wv::IInputListener
{
public:
	 PlayerShip( wv::Mesh* _mesh );
	~PlayerShip();

	void onCreate();
	void onMouseEvent( wv::MouseEvent _event ) override;
	void onInputEvent( wv::InputEvent _event ) override;
	void update( double _deltaTime ) override;

private:

	wv::ICamera* m_camera;
	wv::Vector2f m_aimInput;

	wv::Vector3f m_targetRotation;
	wv::Vector3f m_cameraRotation;
};