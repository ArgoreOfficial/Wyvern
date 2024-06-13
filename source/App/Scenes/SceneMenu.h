#pragma once

#include <wv/Scene/Scene.h>
#include <wv/Events/IInputListener.h>
#include <wv/Math/Vector3.h>

#include <vector>

namespace wv { class Mesh; }
namespace wv { class Material; }
namespace wv { class ICamera; }

struct MenuSprite
{
	wv::Material* material;
	wv::Vector3f position;
	wv::Vector3f size;
};

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
	void draw() override;

private:

	void addSprite( const char* _material, int _x, int _y, int _w, int _h );

	wv::Mesh* m_quad;
	std::vector<MenuSprite> m_sprites;
	wv::ICamera* m_camera;


	int m_selected = 0;
	bool m_blinker = false;
	float m_blinkerTimer = 0.0f;
	bool m_switching = false;
	int m_switchCooldown = 1;
};