#pragma once


#include "Ship.h"

#include <wv/Events/IMouseListener.h>
#include <wv/Events/IInputListener.h>

namespace wv { class ICamera; }
namespace wv { class Audio; }

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

	bool m_playedDeathSound = false;
	float m_deathTimeout = 2.0f;

	wv::ICamera* m_camera;
	wv::Vector2f m_aimInput;
	
	bool m_usingKeyboard = false;
	bool m_invertPitch = true;
	int m_throttleInput = 0;

	wv::Audio* m_engineSound = nullptr;
	wv::Audio* m_deathSound = nullptr;

	wv::Vector3f m_cameraRotation;
};