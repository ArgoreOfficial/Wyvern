#pragma once

#include <wv/Scene/Scene.h>
#include <App/Ship/Ship.h>
#include <App/Ship/PlayerShip.h>
#include <App/Ship/EnemyShip.h>

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
	wv::Mesh* m_xwing = nullptr;
	wv::Mesh* m_starDestroyer  = nullptr;

	PlayerShip* m_playerShip = nullptr;
	EnemyShip* m_dummy = nullptr;

	wv::Audio* m_startupSound = nullptr;
	bool m_hasPlayedStartup = true;

};