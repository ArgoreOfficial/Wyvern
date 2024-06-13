#pragma once

#include <wv/Scene/Scene.h>
#include <wv/Events/IInputListener.h>

namespace wv { class ICamera; }
namespace wv { class Sprite; }

class SceneTexture : public wv::Scene, public wv::IInputListener
{
public:
	 SceneTexture();
	~SceneTexture();

	void onLoad() override;
	void onUnload() override;

	void onCreate() override;
	void onDestroy() override;

	void onInputEvent( wv::InputEvent _event ) override;

	void update( double _deltaTime ) override;
	void draw( wv::GraphicsDevice* _device ) override;

private:

	wv::Sprite* m_sprite;
	wv::ICamera* m_camera;

};