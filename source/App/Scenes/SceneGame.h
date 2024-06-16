#pragma once

#include <wv/Scene/Scene.h>
#include <wv/Events/IInputListener.h>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv { class Mesh; }
namespace wv { class Material; }

///////////////////////////////////////////////////////////////////////////////////////

class DefaultScene : public wv::Scene, public wv::IInputListener
{

public:

	DefaultScene();
	~DefaultScene();

	void onLoad() override;
	void onUnload() override;

	void onCreate() override;
	void onDestroy() override;

	void onInputEvent( wv::InputEvent _event ) override;

	void update( double _deltaTime ) override;
	void draw( wv::GraphicsDevice* _device ) override;

///////////////////////////////////////////////////////////////////////////////////////

private:

	wv::Mesh* m_skybox = nullptr;
	wv::Material* m_skyMaterial = nullptr;

};