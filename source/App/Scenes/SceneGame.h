#pragma once

#include <wv/Scene/Scene.h>
#include <App/Ship/PlayerShip.h>
#include <App/Ship/EnemyShip.h>
#include <App/Ship/StarDestroyer.h>

namespace wv { class Mesh; }
namespace wv { class Audio; }

class SceneGame : public wv::Scene
{
public:
	 SceneGame();
	~SceneGame();

	void onLoad() override;
	void onUnload() override;

	void onCreate() override;
	void onDestroy() override;

	void update( double _deltaTime ) override;
	void draw() override;

private:

	wv::Mesh* m_skybox = nullptr;

	// meshes //
	wv::Mesh* m_xwingMesh = nullptr;
	wv::Mesh* m_starDestroyerMesh = nullptr;

	PlayerShip* m_playerShip = nullptr;
	EnemyShip* m_dummy = nullptr;
	
	wv::Audio* m_backgroundMusic = nullptr;

	std::vector<StarDestroyer*> m_starDestroyers;
};