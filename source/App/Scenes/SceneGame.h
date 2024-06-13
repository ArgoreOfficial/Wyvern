#pragma once

#include <wv/Scene/Scene.h>
#include <App/Ship/PlayerShip.h>
#include <App/Ship/EnemyShip.h>
#include <App/Ship/StarDestroyer.h>

#include <wv/Events/IInputListener.h>

namespace wv { class Mesh; }
namespace wv { class Audio; }
namespace wv { class Material; }

class SceneGame : public wv::Scene, public wv::IInputListener
{
public:
	 SceneGame();
	~SceneGame();

	void onLoad() override;
	void onUnload() override;

	void onCreate() override;
	void onDestroy() override;

	void onInputEvent( wv::InputEvent _event ) override;

	void update( double _deltaTime ) override;
	void draw( wv::GraphicsDevice* _device ) override;

private:

	wv::Mesh* m_skybox = nullptr;

	// meshes //
	wv::Mesh* m_xwingMesh = nullptr;
	wv::Mesh* m_starDestroyerMesh = nullptr;

	PlayerShip* m_playerShip = nullptr;
	EnemyShip* m_dummy = nullptr;
	
	wv::Audio* m_backgroundMusic = nullptr;

	std::vector<StarDestroyer*> m_starDestroyers;

	wv::Material* m_skyMaterial;
};