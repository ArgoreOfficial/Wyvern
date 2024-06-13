#pragma once

#include <wv/Scene/Scene.h>
#include <wv/Events/IInputListener.h>
#include <wv/Math/Vector3.h>

#include <vector>

namespace wv { class Mesh; }
namespace wv { class Material; }
namespace wv { class ICamera; }
namespace wv { class Sprite; }

class SceneMenu : public wv::Scene, public wv::IInputListener
{
public:
	 SceneMenu();
	~SceneMenu();

	void onLoad() override;
	void onUnload() override;

	void onCreate() override;
	void onDestroy() override;

	void onInputEvent( wv::InputEvent _event ) override;

	void update( double _deltaTime ) override;
	void draw( wv::GraphicsDevice* _device ) override;

private:

	void addSprite( const char* _material, int _x, int _y, int _w, int _h );

	std::vector<wv::Sprite*> m_sprites;
	wv::ICamera* m_camera;


	int m_selected = 0;
	bool m_blinker = false;
	float m_blinkerTimer = 0.0f;
	bool m_switching = false;
	int m_switchCooldown = 1;
};