#pragma once

#include <wv/Scene/Scene.h>
#include <wv/Events/IInputListener.h>

namespace wv { class ICamera; }
namespace wv { class Sprite; }
namespace wv { class Mesh; }

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

	wv::Mesh* createQuad( int _numFrames );

	wv::Sprite* m_sprite;
	wv::ICamera* m_camera;

	float m_timer = 0.0f;
	int m_frame = 0;
};